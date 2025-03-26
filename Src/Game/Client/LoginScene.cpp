#include "Stdafx.h"
#include "App.h"
#include "LoginScene.h"
#include "LoginWindow.h"
#include "SceneManager.h"
#include "WindowManager.h"

LoginScene::LoginScene()
{
	auto window{ std::make_shared<LoginWindow>()};
	WindowManager::GetInstance()->Register(window);
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
