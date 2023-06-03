#include "Stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "Image.h"
#include "Property.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player() :
	m_inputComponent{ this },
	m_physicsComponent{ this },
	m_collisionComponent{ this },
	m_animationComponent{ this }
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = static_cast<int>(Layer::LOCALPLAYER);
	m_cbGameObject->alpha = 1.0f;
	m_cbGameObject->isFliped = FALSE;
	
	DirectX::XMFLOAT4X4 worldMatrix{};
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	m_cbGameObject->worldMatrix = worldMatrix;
	
	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::Type::DEFAULT);
	m_shader = rm->GetShader(Shader::Type::DEFAULT);
}

void Player::Update(FLOAT deltaTime)
{
	m_inputComponent.Update(deltaTime);		// 1. 입력 처리
	m_physicsComponent.Update(deltaTime);	// 2. 이동 처리
	m_collisionComponent.Update(deltaTime);	// 3. 충돌 처리
	m_animationComponent.Update(deltaTime);	// 4. 애니메이션
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_animationComponent.SetShaderVariable(commandList);
	if (m_cbGameObject.IsValid())
		m_cbGameObject.SetShaderVariable(commandList, RootParamIndex::GAMEOBJECT);
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_mesh)
		m_mesh->Render(commandList);
}

Player::AnimationComponent::AnimationComponent(Player* player) : 
	m_player{ player },
	m_type{ AnimationType::IDLE },
	m_frame{ 0 },
	m_timer{ 0.0f },
	m_root{ nullptr },
	m_currAniProp{ nullptr },
	m_currFrameProp{ nullptr }
{
	auto rm{ ResourceManager::GetInstance() };
	m_root = rm->Load("Player.nyt");
	PlayAnimation(AnimationType::IDLE);
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
				OnAnimationEnd();
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
	// 좌우 이동에 대한 입력 처리
	int dir{ 0 };
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		--dir;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		++dir;

	// 점프 입력 처리
	bool isJumped{ false };
	if (GetAsyncKeyState('C') & 0x8000)
	{
		m_player->m_physicsComponent.Jump();
		isJumped = true;
	}

	// 좌우 방향 설정
	m_player->m_physicsComponent.Move(static_cast<PhysicsComponent::Direction>(dir));

	// 애니메이션 재생
	auto animationType{ AnimationComponent::AnimationType::NONE };
	if (isJumped)
		animationType = AnimationComponent::AnimationType::JUMP;
	else if (m_player->m_animationComponent.GetAnimationType() != AnimationComponent::AnimationType::JUMP &&
		m_player->m_animationComponent.GetAnimationType() != AnimationComponent::AnimationType::FALL)
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

Player::CollisionComponent::CollisionComponent(Player* player) :
	m_player{ player }
{
}

void Player::CollisionComponent::Update(FLOAT deltaTime)
{
}

Player::PhysicsComponent::PhysicsComponent(Player* player) :
	m_player{ player },
	m_direction{ Direction::NONE },
	m_speed{ 100.0f, 0.0f }
{
}

void Player::PhysicsComponent::OnLanding()
{
	m_player->m_position.y = 0.0f;
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

void Player::PhysicsComponent::Update(FLOAT deltaTime)
{
	m_player->Move(FLOAT2{ static_cast<int>(m_direction) * m_speed.x * deltaTime, m_speed.y * deltaTime });

	// 중력 적용
	m_speed.y -= 980.0f * deltaTime;

	// 착지
	if (m_player->m_position.y < 0.0f)
		OnLanding();
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
