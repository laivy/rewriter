#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const;

private:

};