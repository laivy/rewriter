#pragma once
#include "Scene.h"

class LoginScene final :
	public IScene,
	public IObserver
{
public:
	LoginScene();
	~LoginScene() = default;

	void Update(float deltaTime) override;
	void Render2D() const override;
	void Render3D() const override;

private:
	void OnPacket(Packet& packet);
};
