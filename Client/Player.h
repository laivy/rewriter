#pragma once
#include "GameObject.h"

class NytProperty;

class Player : public IGameObject
{
public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const;

private:
	NytProperty* m_stand;
	FLOAT m_frame;
};