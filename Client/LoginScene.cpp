#include "Stdafx.h"
#include "Camera.h"
#include "LoginScene.h"
#include "NytLoader.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "NytUI.h"
#include "WndManager.h"

LoginScene::LoginScene() : m_prop{}, m_camera{}
{

}

void LoginScene::OnCreate()
{
	m_prop = NytLoader::GetInstance()->Load("Main.nyt");
	m_camera = std::make_unique<Camera>();
}

void LoginScene::OnDestory()
{
	NytLoader::GetInstance()->Unload("Login.nyt");
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

void LoginScene::Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	
}

void LoginScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	WndManager::GetInstance()->Render(d2dContext);

	m_prop->Get<NytUI>("UIStatus/ui")->Render(d2dContext);
}