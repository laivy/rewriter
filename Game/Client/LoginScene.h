#pragma once
#include "Scene.h"

class LoginScene :
	public IScene,
	public IObserver
{
public:
	LoginScene();
	~LoginScene() = default;

	virtual void Update(float deltaTime) override final;
	virtual void Render2D() const override final;
	virtual void Render3D() const override final;

private:
	void OnPacket(Packet& packet);
};