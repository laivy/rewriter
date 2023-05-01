#pragma once
#include "GameObject.h"

class NytProperty;

class Player : public IGameObject
{
public:
	class AnimationComponent
	{
	public:
		enum class Type
		{
			STAND, ATTACK1, ATTACK2
		};

	public:
		AnimationComponent(Player* player);
		~AnimationComponent() = default;

		void OnAnimationStart();
		void OnAnimationEnd();

		void Update(FLOAT deltaTime);

		void PlayAnimation(Type type);
		void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	private:
		static constexpr auto FRAME_INTERVAL = 0.2f;

		Player* m_player;

		Type m_type;
		INT m_frame;
		FLOAT m_timer;
		NytProperty* m_root;
		NytProperty* m_currAniProp;
	};

	class InputComponent
	{
	public:
		InputComponent(Player* player);
		~InputComponent() = default;

		void Update(FLOAT deltaTime);

	private:
		static constexpr auto SPEED = 50.0f;

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
};