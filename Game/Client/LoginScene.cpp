#include "Stdafx.h"
#include "App.h"
#include "LoginScene.h"

LoginScene::LoginScene()
{
	App::OnPacket.Register(this, std::bind_front(&LoginScene::OnPacket, this));
}

void LoginScene::Update(float deltaTime)
{
	IScene::Update(deltaTime);
}

void LoginScene::Render2D() const
{
	IScene::Render2D();
}

void LoginScene::Render3D() const
{
}

void LoginScene::OnPacket(Packet& packet)
{
}
