#pragma once
#include "Scene.h"

class LoginScene final :
	public IScene,
	public IObserver
{
public:
	LoginScene();
	~LoginScene() = default;

	void Update(float deltaTime) override final;
	void Render2D() const override final;
	void Render3D() const override final;

private:
	void OnPacket(Packet& packet);
};
