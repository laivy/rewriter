#include "Stdafx.h"
#include "Map.h"
#include "Mesh.h"
#include "Image.h"
#include "ObjectManager.h"
#include "Platform.h"
#include "Player.h"
#include "Property.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::InputComponent::InputComponent(Player* player) :
	Player::IComponent{ player },
	m_inputDirection{ Direction::NONE }
{
}

void Player::InputComponent::OnMove(Direction direction)
{
	m_inputDirection = static_cast<Direction>(direction);
}

void Player::InputComponent::Update(float deltaTime)
{
	// 이동
	if (m_player->IsCanMove())
	{
		int dir{ 0 };
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
			--dir;
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
			++dir;
		m_player->OnMove(static_cast<Direction>(dir));
	}

	// 점프
	if (m_player->IsCanJump() && GetAsyncKeyState('C') & 0x8000)
		m_player->OnJump();
}

IGameObject::Direction Player::InputComponent::GetInputDirection() const
{
	return m_inputDirection;
}

Player::PhysicsComponent::PhysicsComponent(Player* player) :
	Player::IComponent{ player },
	m_platform{},
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
	m_player->m_speed.y = 0.0f;
	m_isOnPlatform = true;
	m_isJumping = false;
}

void Player::PhysicsComponent::OnFalling()
{
	m_isOnPlatform = false;
}

void Player::PhysicsComponent::Update(float deltaTime)
{
	FLOAT2 beforePosition{ m_player->GetPosition() };
	FLOAT2 afterPosition{};
	std::shared_ptr<Platform> beforePlatform{ m_platform.lock() };
	std::shared_ptr<Platform> afterPlatform{};

	UpdateMovement(deltaTime);

	// 현재 위치에서 가장 높은 플렛폼 계산
	afterPosition = m_player->GetPosition();
	afterPlatform = ObjectManager::GetInstance()->GetMap().lock()->GetBelowPlatform(afterPosition).lock();

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
	m_player->m_speed.y = std::max(m_player->m_speed.y - GRAVITY * deltaTime, MIN_Y_SPEED);
}

void Player::PhysicsComponent::OnMove(Direction direction)
{
	m_player->m_speed.x = PhysicsComponent::DEFAULT_X_SPEED * static_cast<int>(direction);
}

void Player::PhysicsComponent::OnJump()
{
	m_player->m_speed.y = DEFAULT_JUMP_POWER;
	m_isOnPlatform = false;
	m_isJumping = true;
}

bool Player::PhysicsComponent::IsCanJump() const
{
	return !m_isJumping;
}

void Player::PhysicsComponent::UpdateMovement(float deltaTime)
{
	// 플렛폼 위에 있지 않은 경우
	if (!m_isOnPlatform)
	{
		m_player->Move({ m_player->m_speed.x * deltaTime, m_player->m_speed.y * deltaTime });
		return;
	}

	// 플렛폼 위에서 움직이는 경우에는 x축으로만 움직인 뒤, 여전히 같은 플렛폼 위에 있다면 높이를 해당 위치의 플렛폼 높이로 설정
	// 그 외의 경우엔 기존 플렛폼에서 벗어나는 것이므로 y축으로도 움직임
	m_player->Move({ m_player->m_speed.x * deltaTime, 0.0f });

	FLOAT2 pos{ m_player->GetPosition() };
	auto platform{ m_platform.lock() };
	if (platform && platform->IsBetweenX(pos.x))
		m_player->SetPosition({ pos.x, platform->GetHeight(pos.x) });
	else
		m_player->Move({ 0.0f, m_player->m_speed.y * deltaTime });
	m_player->m_speed.y = 0.0f;
}

Player::AnimationComponent::AnimationComponent(Player* player) : 
	Player::IComponent{ player },
	m_type{ AnimationType::IDLE },
	m_frame{ 0 },
	m_timer{ 0.0f },
	m_root{ nullptr },
	m_currAniProp{ nullptr },
	m_currFrameProp{ nullptr }
{
	auto rm{ ResourceManager::GetInstance() };
	m_root = rm->Load("Player.nyt");
	PlayAnimation(AnimationType::FALL);
}

void Player::AnimationComponent::OnMove(Direction direction)
{
	using enum AnimationType;
	using enum Direction;

	m_player->SetDirection(direction);

	if (m_type == JUMP || m_type == FALL)
		return;

	switch (direction)
	{
	case LEFT:
	case RIGHT:
		if (m_type != RUN)
			PlayAnimation(RUN);
		break;
	case NONE:
		if (m_type != IDLE)
			PlayAnimation(IDLE);
		break;
	}
}

void Player::AnimationComponent::OnJump()
{
	PlayAnimation(AnimationType::JUMP);
}

void Player::AnimationComponent::OnLanding()
{
	if (m_player->m_inputComponent.GetInputDirection() == Direction::NONE)
		PlayAnimation(AnimationType::IDLE);
	else
		PlayAnimation(AnimationType::RUN);
}

void Player::AnimationComponent::OnFalling()
{
	PlayAnimation(AnimationType::FALL);
}

void Player::AnimationComponent::OnAnimationStart(AnimationType type)
{

}

void Player::AnimationComponent::OnAnimationEnd(AnimationType type)
{
	using enum AnimationType;
	switch (m_type)
	{
	case IDLE:
	case RUN:
	case FALL:
		PlayAnimation(m_type);
		break;
	case JUMP:
		PlayAnimation(FALL);
		break;
	default:
		break;
	}
}

void Player::AnimationComponent::OnAnimationFrameChange(AnimationType type, int frame)
{
	m_currFrameProp = m_currAniProp->Get<Property>(std::to_string(frame));
}

void Player::AnimationComponent::Update(float deltaTime)
{
	UpdateFrame(deltaTime);
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

	m_player->OnAnimationStart(m_type);
	m_player->OnAnimationFrameChange(m_type, m_frame);
}

void Player::AnimationComponent::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_currAniProp->Get<Image>(std::to_string(m_frame))->SetShaderVariable(commandList);
}

void Player::AnimationComponent::UpdateFrame(float deltaTime)
{
	m_timer += deltaTime;

	FLOAT interval{ DEFAULT_FRAME_INTERVAL };
	do
	{
		if (auto currFrameInterval{ m_currFrameProp->Get<FLOAT>(StringTable::INTERVAL) })
			interval = *currFrameInterval;

		if (m_timer >= interval)
		{
			if (m_frame >= m_currAniProp->GetChildCount() - 1)
			{
				// OnAnimationEnd에서 PlayAnimation을 호출하여 m_timer값이 0이되버리므로 저장해줬다가 다시 설정해줌
				FLOAT timer{ m_timer };
				m_player->OnAnimationEnd(m_type);
				m_timer = timer - interval;
				continue;
			}

			++m_frame;
			m_timer -= interval;
			m_player->OnAnimationFrameChange(m_type, m_frame);
		}
	} while (m_timer >= interval);
}

Player::Player() :
	m_inputComponent{ this },
	m_physicsComponent{ this },
	m_animationComponent{ this },
	m_characterID{ 0 }
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = Layer::LOCALPLAYER;

	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::Type::DEFAULT);
	m_shader = rm->GetShader(Shader::Type::DEFAULT);

	SetSize({ 50.0f, 37.0f });
	SetPivot(Pivot::CENTERBOT);
}

void Player::Update(float deltaTime)
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

CharacterID Player::GetCharacterID() const
{
	return m_characterID;
}

bool Player::IsCanMove() const
{
	return true;
}

bool Player::IsCanJump() const
{
	return m_physicsComponent.IsCanJump();
}

void Player::OnMove(Direction direction)
{
	m_inputComponent.OnMove(direction);
	m_physicsComponent.OnMove(direction);
	m_animationComponent.OnMove(direction);
}

void Player::OnJump()
{
	m_inputComponent.OnJump();
	m_physicsComponent.OnJump();
	m_animationComponent.OnJump();
}

void Player::OnLanding()
{
	m_inputComponent.OnLanding();
	m_physicsComponent.OnLanding();
	m_animationComponent.OnLanding();
}

void Player::OnFalling()
{
	m_inputComponent.OnFalling();
	m_physicsComponent.OnFalling();
	m_animationComponent.OnFalling();
}

void Player::OnAnimationStart(AnimationType type)
{
	m_inputComponent.OnAnimationStart(type);
	m_physicsComponent.OnAnimationStart(type);
	m_animationComponent.OnAnimationStart(type);
}

void Player::OnAnimationEnd(AnimationType type)
{
	m_inputComponent.OnAnimationEnd(type);
	m_physicsComponent.OnAnimationEnd(type);
	m_animationComponent.OnAnimationEnd(type);
}

void Player::OnAnimationFrameChange(AnimationType type, int frame)
{
	m_inputComponent.OnAnimationFrameChange(type, frame);
	m_physicsComponent.OnAnimationFrameChange(type, frame);
	m_animationComponent.OnAnimationFrameChange(type, frame);
}
