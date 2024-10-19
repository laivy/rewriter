#include "Stdafx.h"
#include "App.h"
#include "DebugWindow.h"
#include "LoginScene.h"
#include "WindowBuilder.h"
#include "WindowManager.h"

LoginScene::LoginScene()
{
	App::OnPacket.Register(this, std::bind_front(&LoginScene::OnPacket, this));

#ifdef _DEBUG
	auto window = WindowBuilder<DebugWindow>{}
		//.Path(L"UI.dat/LoginUI")
		.Position({ App::size.x / 2, App::size.y / 2 }, Pivot::CENTER)
		.Build();
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
