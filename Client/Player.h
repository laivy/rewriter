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
		static constexpr auto GRAVITY = 980.0f;
		static constexpr auto MIN_Y_SPEED = -980.0f;
		static constexpr auto DEFAULT_X_SPEED = 150.0f;
		static constexpr auto DEFAULT_JUMP_POWER = 450.0f;

	public:
		PhysicsComponent(Player* player);
		~PhysicsComponent() = default;

		void OnJump();
		void OnLanding();
		void OnFalling();

		void Update(FLOAT deltaTime);

		bool CanJump() const;

	private:
		void UpdateMovement(float deltaTime);

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

	CharacterID GetCharacterID() const;

private:
	void OnJump();
	void OnLanding();
	void OnFalling();

	void SetDirection(Direction direction);

	Player::Direction GetDirection() const;

private:
	CharacterID m_characterID;

	Direction m_direction;

	InputComponent m_inputComponent;
	PhysicsComponent m_physicsComponent;
	AnimationComponent m_animationComponent;
};

class LocalPlayer : public Player { };
class RemotePlayer : public Player { };