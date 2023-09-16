#pragma once
#include "GameObject.h"

class Platform;
class Property;

class Player : public IGameObject
{
private:
	enum class AnimationType
	{
		IDLE, RUN, JUMP, FALL
	};

	class IComponent abstract
	{
	public:
		IComponent(Player* player) : m_player{ player } { }
		virtual ~IComponent() = default;

		virtual void OnMove(Direction direction) { }
		virtual void OnJump() { }
		virtual void OnLanding() { }
		virtual void OnFalling() { }
		virtual void OnAnimationStart(AnimationType type) { }
		virtual void OnAnimationEnd(AnimationType type) { }
		virtual void OnAnimationFrameChange(AnimationType type, int frame) { }
		virtual void Update(float deltaTime) { }

	protected:
		Player* m_player;
	};

	class InputComponent : public IComponent
	{
	public:
		InputComponent(Player* player);
		~InputComponent() = default;

		virtual void OnMove(Direction direction) final;
		virtual void Update(float deltaTime) final;

		Direction GetInputDirection() const;

	private:
		Direction m_inputDirection; // 지금 이동 방향 (좌(-1), 제자리(0), 우(+1))
	};

	class PhysicsComponent : public IComponent
	{
	public:
		static constexpr auto GRAVITY = 980.0f;
		static constexpr auto MIN_Y_SPEED = -980.0f;
		static constexpr auto DEFAULT_X_SPEED = 150.0f;
		static constexpr auto DEFAULT_JUMP_POWER = 450.0f;

	public:
		PhysicsComponent(Player* player);
		~PhysicsComponent() = default;

		virtual void OnMove(Direction direction) final;
		virtual void OnJump() final;
		virtual void OnLanding() final;
		virtual void OnFalling() final;
		virtual void Update(float deltaTime) final;

		bool IsCanJump() const;

	private:
		void UpdateMovement(float deltaTime);

	private:
		std::weak_ptr<Platform> m_platform;		
		bool m_isOnPlatform;
		bool m_isJumping;
	};

	class AnimationComponent : public IComponent
	{
	public:
		static constexpr auto DEFAULT_FRAME_INTERVAL = 0.2f;

	public:
		AnimationComponent(Player* player);
		~AnimationComponent() = default;

		virtual void OnMove(Direction direction) final;
		virtual void OnJump() final;
		virtual void OnLanding() final;
		virtual void OnFalling() final;
		virtual void OnAnimationStart(AnimationType type) final;
		virtual void OnAnimationEnd(AnimationType type) final;
		virtual void OnAnimationFrameChange(AnimationType type, int frame) final;
		virtual void Update(float deltaTime) final;

		void PlayAnimation(AnimationType type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	private:
		void UpdateFrame(float deltaTime);

	private:
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
	void OnAnimationStart(AnimationType type);
	void OnAnimationEnd(AnimationType type);
	void OnAnimationFrameChange(AnimationType type, int frame);

private:
	CharacterID m_characterID;
	InputComponent m_inputComponent;
	PhysicsComponent m_physicsComponent;
	AnimationComponent m_animationComponent;
};

class LocalPlayer : public Player { };
class RemotePlayer : public Player { };