#include "Stdafx.h"
#include "App.h"
#include "DebugWindow.h"
#include "LoginScene.h"
#include "WindowManager.h"

LoginScene::LoginScene()
{
#ifdef _DEBUG
	auto window{ std::make_shared<DebugWindow>(L"UI.dat/Debug")};
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
