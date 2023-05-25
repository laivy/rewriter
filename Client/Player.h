#pragma once
#include "GameObject.h"

class Property;

class Player : public IGameObject
{
public:
	class AnimationComponent
	{
	public:
		enum class AnimationType
		{
			STAND, ATTACK1, ATTACK2
		};

	public:
		AnimationComponent(Player* player);
		~AnimationComponent() = default;

		void OnAnimationStart();
		void OnAnimationEnd();

		void Update(FLOAT deltaTime);

		void PlayAnimation(AnimationType type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	private:
		static constexpr auto FRAME_INTERVAL = 0.2f;

		Player* m_player;

		AnimationType m_type;
		int m_frame;
		float m_timer;
		Property* m_root;
		Property* m_currAniProp;
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

	class CollisionComponent
	{
	public:
		CollisionComponent(Player* player);
		~CollisionComponent() = default;

		void Update(FLOAT deltaTime);

	private:
		Player* m_player;
	};

public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

private:
	AnimationComponent m_animationComponent;
	InputComponent m_inputComponent;
	CollisionComponent m_collisionComponent;

	bool m_isOnPlatform;
	int m_speed;
};