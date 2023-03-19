#include "Stdafx.h"
#include "Camera.h"
#include "LoginScene.h"
#include "LoginWnd.h"
#include "Mesh.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "NytUI.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "WndManager.h"

void LoginScene::OnCreate()
{
	std::unique_ptr<LoginWnd> wnd{ new LoginWnd{ 300.0f, 300.0f } };
	WndManager::GetInstance()->AddWnd(wnd);
}

void LoginScene::OnDestory()
{
	ResourceManager::GetInstance()->Unload("Login.nyt");
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