#pragma once
#include "GameObject.h"

class Platform;
class Property;

class Player : public IGameObject
{
public:
	enum class Direction
	{
		LEFT = -1,
		NONE = 0,
		RIGHT = 1
	};

public:
	class InputComponent
	{
	public:
		InputComponent(Player* player);
		~InputComponent() = default;

		void Update(FLOAT deltaTime);

	private:
		Player* m_player;
	};

	class PhysicsComponent
	{
	public:
		constexpr static auto GRAVITY = 980.0f;
		constexpr static auto MIN_Y_SPEED = -980.0f;

	public:
		PhysicsComponent(Player* player);
		~PhysicsComponent() = default;

		void OnJump();
		void OnLanding();
		void OnFalling();

		void Update(FLOAT deltaTime);

		bool CanJump() const;

	private:
		std::weak_ptr<Platform> GetTopPlatformBelowPosition(const FLOAT2& position) const;

	private:
		Player* m_player;

		std::weak_ptr<Platform> m_platform;
		FLOAT2 m_speed;
		bool m_isOnPlatform;
		bool m_isJumping;
	};

	class AnimationComponent
	{
	public:
		constexpr static auto DEFAULT_FRAME_INTERVAL = 0.2f;

		enum class AnimationType
		{
			NONE,
			IDLE,
			RUN,
			JUMP,
			FALL
		};

	public:
		AnimationComponent(Player* player);
		~AnimationComponent() = default;

		void OnJump();
		void OnLanding();
		void OnFalling();

		void OnAnimationStart();
		void OnAnimationEnd();
		void OnFrameChange();

		void Update(FLOAT deltaTime);

		void PlayAnimation(AnimationType type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

		AnimationType GetAnimationType() const;

	private:
		void UpdateAnimationType(float deltaTime);
		void UpdateAnimationFrame(float deltaTime);

	private:
		Player* m_player;

		AnimationType m_type;
		INT m_frame;
		FLOAT m_timer;
		Property* m_root;
		Property* m_currAniProp;
		Property* m_currFrameProp;
	};

public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

private:
	void OnJump();
	void OnLanding();
	void OnFalling();

	void SetDirection(Direction direction);
	Player::Direction GetDirection() const;

private:
	InputComponent m_inputComponent;
	PhysicsComponent m_physicsComponent;
	AnimationComponent m_animationComponent;

	Direction m_direction;
};