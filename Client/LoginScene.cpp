#include "Stdafx.h"
#include "Camera.h"
#include "EditCtrl.h"
#include "EventManager.h"
#include "Label.h"
#include "LoginScene.h"
#include "LoginWnd.h"
#include "Mesh.h"
#include "Image.h"
#include "Property.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "WndManager.h"

void LoginScene::OnCreate()
{
	auto wnd{ std::make_unique<LoginWnd>(INT2{ 230, 300 }) };
	wnd->SetPosition({ 1920 / 2, 1080 / 2 });

	auto wnd2{ std::make_unique<LoginWnd>(INT2{ 230, 300 }) };
	wnd2->SetPosition({ 1920 / 2 - 235, 1080 / 2 });

	auto wnd3{ std::make_unique<LoginWnd>(INT2{ 230, 300 }) };
	wnd3->SetPosition({ 1920 / 2 + 235, 1080 / 2 });

	WndManager::GetInstance()->AddWnd(wnd.release());
	WndManager::GetInstance()->AddWnd(wnd2.release());
	WndManager::GetInstance()->AddWnd(wnd3.release());
}

void LoginScene::OnDestory()
{
	ResourceManager::GetInstance()->Unload("Login.nyt");
}

void LoginScene::OnMouseMove(int x, int y)
{
	WndManager::GetInstance()->OnMouseMove(x, y);
}

void LoginScene::OnLButtonDown(int x, int y)
{
	WndManager::GetInstance()->OnLButtonDown(x, y);
}

void LoginScene::OnLButtonUp(int x, int y)
{
	WndManager::GetInstance()->OnLButtonUp(x, y);
}

void LoginScene::OnRButtonDown(int x, int y)
{
	WndManager::GetInstance()->OnRButtonDown(x, y);
}

void LoginScene::OnRButtonUp(int x, int y)
{
	WndManager::GetInstance()->OnRButtonUp(x, y);
}

void LoginScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	WndManager::GetInstance()->OnKeyboardEvent(message, wParam, lParam);
}

void LoginScene::Update(FLOAT deltaTime)
{
	WndManager::GetInstance()->Update(deltaTime);
}

void LoginScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{

}

void LoginScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	WndManager::GetInstance()->Render(d2dContext);
}