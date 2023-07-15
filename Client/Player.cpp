#include "Stdafx.h"
#include "GameScene.h"
#include "Map.h"
#include "Mesh.h"
#include "Image.h"
#include "Platform.h"
#include "Player.h"
#include "Property.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player() :
	m_inputComponent{ this },
	m_physicsComponent{ this },
	m_animationComponent{ this }
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = static_cast<int>(Layer::LOCALPLAYER);
	m_cbGameObject->alpha = 1.0f;
	m_cbGameObject->isFliped = FALSE;
	DirectX::XMStoreFloat4x4(&m_cbGameObject->worldMatrix, DirectX::XMMatrixIdentity());
	
	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::Type::DEFAULT);
	m_shader = rm->GetShader(Shader::Type::DEFAULT);
}

void Player::Update(FLOAT deltaTime)
{
	m_inputComponent.Update(deltaTime);		// 1. 입력 처리
	m_physicsComponent.Update(deltaTime);	// 2. 이동 처리
	m_animationComponent.Update(deltaTime);	// 3. 애니메이션
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_animationComponent.SetShaderVariable(commandList);
	if (m_cbGameObject.IsValid())
		m_cbGameObject.SetShaderVariable(commandList, RootParamIndex::GAMEOBJECT);
	if (auto s{ m_shader.lock() })
		commandList->SetPipelineState(s->GetPipelineState());
	if (auto m{ m_mesh.lock() })
		m->Render(commandList);
}

Player::AnimationComponent::AnimationComponent(Player* player) : 
	m_player{ player },
	m_type{ AnimationType::IDLE },
	m_frame{ 0 },
	m_timer{ 0.0f },
	m_root{},
	m_currAniProp{},
	m_currFrameProp{}
{
	auto rm{ ResourceManager::GetInstance() };
	m_root = rm->Load("Player.nyt");
	PlayAnimation(AnimationType::FALL);
}

void Player::AnimationComponent::OnAnimationStart()
{

}

void Player::AnimationComponent::OnAnimationEnd()
{
	switch (m_type)
	{
	case AnimationType::IDLE:
	case AnimationType::RUN:
	case AnimationType::FALL:
		PlayAnimation(m_type);
		break;
	case AnimationType::JUMP:
		PlayAnimation(AnimationType::FALL);
		break;
	default:
		break;
	}
}

void Player::AnimationComponent::OnFrameChange()
{
	m_currFrameProp = m_currAniProp->Get<Property>(std::to_string(m_frame));
}

void Player::AnimationComponent::Update(FLOAT deltaTime)
{
	m_timer += deltaTime;

	FLOAT interval{ DEFAULT_FRAME_INTERVAL };
	do
	{
		if (auto currFrameInterval{ m_currFrameProp->Get<FLOAT>("interval") })
			interval = *currFrameInterval;

		if (m_timer >= interval)
		{
			if (m_frame >= m_currAniProp->GetChildCount() - 1)
			{
				// OnAnimationEnd에서 PlayAnimation을 호출하여 m_timer값이 0이되버리므로 저장해줬다가 다시 설정해줌
				FLOAT timer{ m_timer };
				OnAnimationEnd();
				m_timer = timer - interval;
				continue;
			}

			++m_frame;
			m_timer -= interval;
			OnFrameChange();
		}
	} while (m_timer >= interval);
}

void Player::AnimationComponent::PlayAnimation(AnimationType type)
{
	m_type = type;
	m_frame = 0;
	m_timer = 0.0f;

	switch (m_type)
	{
	case AnimationType::IDLE:
		m_currAniProp = m_root->Get<Property>("Idle");
		break;
	case AnimationType::RUN:
		m_currAniProp = m_root->Get<Property>("Run");
		break;
	case AnimationType::JUMP:
		m_currAniProp = m_root->Get<Property>("Jump");
		break;
	case AnimationType::FALL:
		m_currAniProp = m_root->Get<Property>("Fall");
		break;
	default:
		break;
	}

	OnAnimationStart();
	OnFrameChange();
}

void Player::AnimationComponent::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_currAniProp->Get<Image>(std::to_string(m_frame))->SetShaderVariable(commandList);
}

Player::AnimationComponent::AnimationType Player::AnimationComponent::GetAnimationType() const
{
	return m_type;
}

Player::InputComponent::InputComponent(Player* player) : 
	m_player{ player }
{
}

void Player::InputComponent::Update(FLOAT deltaTime)
{
	// 좌우 이동
	int dir{ 0 };
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		--dir;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		++dir;
	m_player->m_physicsComponent.Move(static_cast<PhysicsComponent::Direction>(dir));

	// 점프
	bool isJumped{ false };
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_player->m_physicsComponent.Jump();
		isJumped = true;
	}

	// 애니메이션 재생
	auto animationType{ AnimationComponent::AnimationType::NONE };
	if (isJumped)
		animationType = AnimationComponent::AnimationType::JUMP;
	else if (m_player->m_animationComponent.GetAnimationType() != AnimationComponent::AnimationType::JUMP && m_player->m_animationComponent.GetAnimationType() != AnimationComponent::AnimationType::FALL)
	{
		if (dir != 0)
			animationType = AnimationComponent::AnimationType::RUN;
		else
			animationType = AnimationComponent::AnimationType::IDLE;
	}

	if (animationType != AnimationComponent::AnimationType::NONE &&
		m_player->m_animationComponent.GetAnimationType() != animationType)
		m_player->m_animationComponent.PlayAnimation(animationType);
}

Player::PhysicsComponent::PhysicsComponent(Player* player) :
	m_player{ player },
	m_platform{ nullptr },
	m_direction{ Direction::NONE },
	m_speed{ 100.0f, 0.0f }
{
}

void Player::PhysicsComponent::OnLanding()
{
	m_player->m_position.y = m_platform->GetHeight(m_player->GetPosition().x);
	m_speed.y = 0.0f;

	if (m_player->m_animationComponent.GetAnimationType() == AnimationComponent::AnimationType::JUMP ||
		m_player->m_animationComponent.GetAnimationType() == AnimationComponent::AnimationType::FALL)
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
			m_player->m_animationComponent.PlayAnimation(AnimationComponent::AnimationType::RUN);
		else
			m_player->m_animationComponent.PlayAnimation(AnimationComponent::AnimationType::IDLE);
	}
}

void Player::PhysicsComponent::OnFalling()
{
	m_player->m_animationComponent.PlayAnimation(AnimationComponent::AnimationType::FALL);
}

void Player::PhysicsComponent::Update(FLOAT deltaTime)
{
	FLOAT2 beforePlayerPosition{ m_player->GetPosition() };
	FLOAT2 afterPlayerPosition{};
	const Platform* beforePlatform{ m_platform };
	const Platform* afterPlatform{ nullptr };

	// 이동. 현재 플렛폼의 높이와 플레이어의 높이가 같다면 플렛폼 위에 서있다는 것
	if (m_speed.y < 0.0f && m_platform && m_platform->GetHeight(beforePlayerPosition.x) == beforePlayerPosition.y)
		m_speed.y = 0.0f;
	m_player->Move(FLOAT2{ static_cast<int>(m_direction) * m_speed.x * deltaTime, m_speed.y * deltaTime });

	// 현재 위치에서 가장 높은 플렛폼 계산
	afterPlayerPosition = m_player->GetPosition();
	afterPlatform = GetTopPlatformBelowPosition(afterPlayerPosition);

	// 움직이기 이전, 이후 플레이어 y좌표 사이에 이전 플렛폼 높이가 있다면 착지한 것
	FLOAT platformHeight{ -FLT_MAX };
	if (beforePlatform)
		platformHeight = beforePlatform->GetHeight(beforePlayerPosition.x);
	if (afterPlayerPosition.y < platformHeight && platformHeight < beforePlayerPosition.y)
	{
		afterPlatform = beforePlatform;
		m_platform = beforePlatform;
		OnLanding();
	}
	else
	{
		m_platform = afterPlatform;
	}

	// 이전, 이후 플렛폼이 다르고 이후 플렛폼이 없거나 이전 플렛폼의 높이가 이후 플렛폼의 높이보다 크면 플렛폼을 벗어나 떨어지는 것
	if (beforePlatform != afterPlatform && (!afterPlatform || (beforePlatform && afterPlatform && beforePlatform->GetHeight(beforePlayerPosition.x) > afterPlatform->GetHeight(afterPlayerPosition.x))))
		OnFalling();

	// 중력 적용
	m_speed.y -= 980.0f * deltaTime;
}

void Player::PhysicsComponent::Move(Direction direction)
{
	m_direction = direction;
	
	switch (m_direction)
	{
	case Direction::LEFT:
		m_player->m_cbGameObject->isFliped = TRUE;
		break;
	case Direction::RIGHT:
		m_player->m_cbGameObject->isFliped = FALSE;
		break;
	}
}

void Player::PhysicsComponent::Jump()
{
	m_speed.y = 300.0f;
}

const Platform* Player::PhysicsComponent::GetTopPlatformBelowPosition(const FLOAT2& position) const
{
	Map* map{ GameScene::GetInstance()->GetMap() };
	if (!map)
		return nullptr;

	const Platform* topPlatform{ nullptr };
	FLOAT topPlatformHeight{ -FLT_MAX };
	FLOAT2 playerPosition{ m_player->GetPosition() };

	for (const auto& p : map->GetPlatforms())
	{
		auto [s, e] { p.GetStartEndPosition() };
		if (position.x < s.x || position.x > e.x)
			continue;

		FLOAT platformHeight{ p.GetHeight(playerPosition.x) };
		if (platformHeight > playerPosition.y ||
			platformHeight < topPlatformHeight)
			continue;

		topPlatformHeight = platformHeight;
		topPlatform = &p;
	}
	return topPlatform;
}
