#include "Stdafx.h"
#include "LoginScene.h"
#include "NytLoader.h"
#include "NytProperty.h"
#include "WndManager.h"

void LoginScene::OnCreate()
{
	// 데이터 로딩
	m_prop = &NytLoader::GetInstance()->Load("Login.nyt");

	//auto w{ std::make_unique<Wnd>(m_prop->Get<int>("Group1/x"), m_prop->Get<int>("Group1/y"), 500.0f, 500.0f) };
	auto w{ std::make_unique<Wnd>(500.0f, 500.0f, 500.0f, 500.0f) };
	auto wm{ WndManager::GetInstance() };
	wm->AddWnd(w);
}

void LoginScene::OnDestory()
{
	NytLoader::GetInstance()->Unload("Login.nyt");
}

void LoginScene::Update(FLOAT deltaTime)
{
	WndManager::GetInstance()->Update(deltaTime);
}

void LoginScene::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	WndManager::GetInstance()->Render(renderTarget);
}