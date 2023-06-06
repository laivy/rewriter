#pragma once
#include "GameObject.h"

class Platform;
class Property;

class Player : public IGameObject
{
public:
	class AnimationComponent
	{
	public:
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

		void OnAnimationStart();
		void OnAnimationEnd();
		void OnFrameChange();

		void Update(FLOAT deltaTime);

		void PlayAnimation(AnimationType type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

		AnimationType GetAnimationType() const;

	private:
		static constexpr auto DEFAULT_FRAME_INTERVAL = 0.2f;

		Player* m_player;

		AnimationType m_type;
		INT m_frame;
		FLOAT m_timer;
		Property* m_root;
		Property* m_currAniProp;
		Property* m_currFrameProp;
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
		enum class Direction
		{
			LEFT = -1,
			NONE = 0,
			RIGHT = 1
		};

	public:
		PhysicsComponent(Player* player);
		~PhysicsComponent() = default;

		void OnLanding();
		void OnFalling();

		void Update(FLOAT deltaTime);

		void Move(Direction direction);
		void Jump();

	private:
		const Platform* GetTopPlatformBelowPosition(const FLOAT2& position);

	private:
		Player* m_player;
		const Platform* m_platform;
		Direction m_direction;
		FLOAT2 m_speed;
	};

public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

private:
	InputComponent m_inputComponent;
	PhysicsComponent m_physicsComponent;
	AnimationComponent m_animationComponent;
};