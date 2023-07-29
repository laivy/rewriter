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
	m_animationComponent{ this },
	m_direction{ Direction::RIGHT }
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = static_cast<int>(Layer::LOCALPLAYER);
	m_cbGameObject->alpha = 1.0f;
	m_cbGameObject->isFliped = FALSE;
	DirectX::XMStoreFloat4x4(&m_cbGameObject->worldMatrix, DirectX::XMMatrixIdentity());
	
	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::Type::DEFAULT);
	m_shader = rm->GetShader(Shader::Type::DEFAULT);

	SetSize({ 50.0f, 37.0f });
	SetPivot(Pivot::CENTERBOT);
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

void Player::OnJump()
{
	m_physicsComponent.OnJump();
	m_animationComponent.OnJump();
}

void Player::OnLanding()
{
	m_physicsComponent.OnLanding();
	m_animationComponent.OnLanding();
}

void Player::OnFalling()
{
	m_physicsComponent.OnFalling();
	m_animationComponent.OnFalling();
}

void Player::SetDirection(Direction direction)
{
	m_direction = direction;
	if (m_cbGameObject.IsValid())
	{
		switch (m_direction)
		{
		case Direction::LEFT:
			m_cbGameObject->isFliped = TRUE;
			break;
		case Direction::RIGHT:
			m_cbGameObject->isFliped = FALSE;
			break;
		}
	}
}

Player::Direction Player::GetDirection() const
{
	return m_direction;
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

void Player::AnimationComponent::OnJump()
{
	PlayAnimation(AnimationType::JUMP);
}

void Player::AnimationComponent::OnLanding()
{
	if (m_player->GetDirection() == Direction::NONE)
		PlayAnimation(AnimationType::IDLE);
	else
		PlayAnimation(AnimationType::RUN);
}

void Player::AnimationComponent::OnFalling()
{
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
	UpdateAnimationType(deltaTime);
	UpdateAnimationFrame(deltaTime);
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

void Player::AnimationComponent::UpdateAnimationType(float deltaTime)
{
	auto type{ AnimationType::NONE };
	if (m_type != AnimationType::JUMP && m_type != AnimationType::FALL)
	{
		if (m_player->GetDirection() != Direction::NONE)
			type = AnimationType::RUN;
		else
			type = AnimationType::IDLE;
	}

	if (type != AnimationType::NONE && m_type != type)
		PlayAnimation(type);
}

void Player::AnimationComponent::UpdateAnimationFrame(float deltaTime)
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
	m_player->SetDirection(static_cast<Direction>(dir));

	// 점프
	if (GetAsyncKeyState('C') & 0x8000 && m_player->m_physicsComponent.CanJump())
	{
		m_player->OnJump();
	}
}

Player::PhysicsComponent::PhysicsComponent(Player* player) :
	m_player{ player },
	m_platform{},
	m_speed{ 100.0f, 0.0f },
	m_isOnPlatform{ false },
	m_isJumping{ false }
{
}

void Player::PhysicsComponent::OnLanding()
{
	if (auto p{ m_platform.lock() })
	{
		m_player->m_position.y = p->GetHeight(m_player->GetPosition().x);
	}
	m_speed.y = 0.0f;
	m_isOnPlatform = true;
	m_isJumping = false;
}

void Player::PhysicsComponent::OnFalling()
{
	m_isOnPlatform = false;
}

void Player::PhysicsComponent::Update(FLOAT deltaTime)
{
	FLOAT2 beforePosition{ m_player->GetPosition() };
	FLOAT2 afterPosition{};
	std::shared_ptr<Platform> beforePlatform{ m_platform.lock() };
	std::shared_ptr<Platform> afterPlatform{};

	UpdateMovement(deltaTime);

	// 현재 위치에서 가장 높은 플렛폼 계산
	afterPosition = m_player->GetPosition();
	afterPlatform = GameScene::GetInstance()->GetMap()->GetBelowPlatform(afterPosition).lock();

	// 움직이기 이전, 이후 플레이어 y좌표 사이에 이전 플렛폼 높이가 있다면 착지한 것
	FLOAT platformHeight{ -FLT_MAX };
	if (beforePlatform)
		platformHeight = beforePlatform->GetHeight(afterPosition.x);
	if (!m_isOnPlatform && afterPosition.y <= platformHeight)
	{
		afterPlatform = beforePlatform;
		m_platform = afterPlatform;
		m_player->OnLanding();
	}
	else
	{
		m_platform = afterPlatform;
	}

	// 플렛폼 위에 있으면서 움직이기 이전, 이후 플렛폼이 다르고 이후 플렛폼이 없거나 이전 플렛폼의 높이가 이후 플렛폼의 높이보다 크면 플렛폼을 벗어나 떨어지는 것
	if (m_isOnPlatform && beforePlatform != afterPlatform && (!afterPlatform || (beforePlatform && afterPlatform && beforePlatform->GetHeight(beforePosition.x) > afterPlatform->GetHeight(afterPosition.x))))
		m_player->OnFalling();

	// 중력 적용
	m_speed.y = std::max(m_speed.y - GRAVITY * deltaTime, MIN_Y_SPEED);
}

void Player::PhysicsComponent::OnJump()
{
	m_speed.y = 500.0f;
	m_isOnPlatform = false;
	m_isJumping = true;
}

bool Player::PhysicsComponent::CanJump() const
{
	return !m_isJumping;
}

void Player::PhysicsComponent::UpdateMovement(float deltaTime)
{
	// 플렛폼 위에 있지 않은 경우
	if (!m_isOnPlatform)
	{
		m_player->Move({ static_cast<int>(m_player->GetDirection()) * m_speed.x * deltaTime, m_speed.y * deltaTime });
		return;
	}

	// 플렛폼 위에서 움직이는 경우에는 x축으로만 움직인 뒤, 여전히 같은 플렛폼 위에 있다면 높이를 해당 위치의 플렛폼 높이로 설정
	// 그 외의 경우엔 기존 플렛폼에서 벗어나는 것이므로 y축으로도 움직임
	m_player->Move({ static_cast<int>(m_player->GetDirection()) * m_speed.x * deltaTime, 0.0f });

	FLOAT2 pos{ m_player->GetPosition() };
	auto platform{ m_platform.lock() };
	if (platform && platform->IsBetweenX(pos.x))
		m_player->SetPosition({ pos.x, platform->GetHeight(pos.x) });
	else
		m_player->Move({ 0.0f, m_speed.y * deltaTime });
	m_speed.y = 0.0f;
}
