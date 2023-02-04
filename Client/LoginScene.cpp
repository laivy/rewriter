#include "Stdafx.h"
#include "Camera.h"
#include "LoginScene.h"
#include "NytLoader.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "WndManager.h"

LoginScene::LoginScene() : m_prop{}, m_camera{}
{

}

void LoginScene::OnCreate()
{
	// 데이터 로딩
	m_prop = &NytLoader::GetInstance()->Load("Login.nyt");
	assert(m_prop);

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

	static float scale = 1.0f;
	scale += deltaTime * 1.0f;
	m_camera->SetScale(FLOAT2{ scale, scale });
}

void LoginScene::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	WndManager::GetInstance()->Render(renderTarget);

	renderTarget->SetTransform(m_camera->GetMatrix());

	auto paimon{ m_prop->Get<NytImage>("Group1/paimon") };
	paimon.Render(renderTarget);

	renderTarget->SetTransform(MATRIX::Identity());
}