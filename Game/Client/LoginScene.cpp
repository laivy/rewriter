#include "Stdafx.h"
#include "App.h"
#include "LoginScene.h"
#include "LoginWindow.h"
#include "WindowManager.h"

LoginScene::LoginScene()
{
#ifdef _DEBUG
	auto window{ std::make_shared<LoginWindow>()};
	WindowManager::GetInstance()->Register(window);
#endif // _DEBUG
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
