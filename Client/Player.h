#pragma once
#include "GameObject.h"

class NytImage;

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const;

private:
	NytImage* m_image;
};