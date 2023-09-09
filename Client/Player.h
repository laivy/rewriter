#pragma once
#include "GameObject.h"

class Platform;
class Property;

class Player : public IGameObject
{
public:
	class InputComponent
	{
	public:
		InputComponent(Player* player);
		~InputComponent() = default;

		void Update(float deltaTime);

		Direction GetInputDirection() const;

	private:
		Player* m_player;
		Direction m_inputDirection; // 키보드 입력에 따른 현재 이동 방향 (좌(-1), 제자리(0), 우(+1))
	};

	class PhysicsComponent
	{
	public:
		constexpr static auto GRAVITY = 980.0f;
		constexpr static auto MIN_Y_SPEED = -980.0f;
		constexpr static auto DEFAULT_X_SPEED = 150.0f;
		constexpr static auto DEFAULT_JUMP_POWER = 450.0f;

	public:
		PhysicsComponent(Player* player);
		~PhysicsComponent() = default;

		void OnMove(Direction direction);
		void OnJump();
		void OnLanding();
		void OnFalling();

		void Update(float deltaTime);

		bool IsCanJump() const;

	private:
		void UpdateMovement(float deltaTime);

	private:
		Player* m_player;
		std::weak_ptr<Platform> m_platform;		
		bool m_isOnPlatform;
		bool m_isJumping;
	};

	class AnimationComponent
	{
	public:
		constexpr static auto DEFAULT_FRAME_INTERVAL = 0.2f;

		enum class AnimationType
		{
			IDLE,
			RUN,
			JUMP,
			FALL
		};

	public:
		AnimationComponent(Player* player);
		~AnimationComponent() = default;

		void OnMove(Direction direction);
		void OnJump();
		void OnLanding();
		void OnFalling();

		void OnAnimationStart(AnimationType type);
		void OnAnimationEnd(AnimationType type);
		void OnAnimationFrameChange(AnimationType type, int frame);

		void Update(float deltaTime);

		void PlayAnimation(AnimationType type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

		AnimationType GetAnimationType() const;

	private:
		void UpdateFrame(float deltaTime);

	private:
		Player* m_player;

		AnimationType m_type;
		int m_frame;
		float m_timer;
		Property* m_root;
		Property* m_currAniProp;
		Property* m_currFrameProp;
	};

public:
	Player();
	~Player() = default;

	virtual void Update(float deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	CharacterID GetCharacterID() const;

	bool IsCanMove() const;
	bool IsCanJump() const;

private:
	void OnMove(Direction direction);
	void OnJump();
	void OnLanding();
	void OnFalling();
	void OnAnimationStart(AnimationComponent::AnimationType type);
	void OnAnimationEnd(AnimationComponent::AnimationType type);
	void OnAnimationFrameChange(AnimationComponent::AnimationType type, int frame);

private:
	CharacterID m_characterID;
	InputComponent m_inputComponent;
	PhysicsComponent m_physicsComponent;
	AnimationComponent m_animationComponent;
};

class LocalPlayer : public Player { };
class RemotePlayer : public Player { };