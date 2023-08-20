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
	WndManager::GetInstance()->AddWnd(wnd.release());
}

void LoginScene::OnDestory()
{
	ResourceManager::GetInstance()->Unload("Login.nyt");
	WndManager::GetInstance()->Clear();
}

void LoginScene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WndManager::GetInstance()->OnMouseEvent(hWnd, message, wParam, lParam);
}

void LoginScene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WndManager::GetInstance()->OnKeyboardEvent(hWnd, message, wParam, lParam);
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