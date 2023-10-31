#include "Stdafx.h"
#include "LoginScene.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "ClientApp.h"
#include "SceneManager.h"

LogoScene::LogoScene() : m_prop{ nullptr }
{
}

void LogoScene::OnCreate()
{
	SceneManager::GetInstance()->SetFadeOut(0.5f,
		[]()
		{
			LoginScene::Instantiate();
			auto sm{ SceneManager::GetInstance() };
			sm->SetScene(LoginScene::GetInstance());
			sm->SetFadeIn(0.5f);
			LogoScene::Destroy();
		}
	);
}

void LogoScene::OnDestory() { }

void LogoScene::OnLButtonDown(int x, int y) { }

void LogoScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) { }

void LogoScene::Update(FLOAT deltaTime) { }

void LogoScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }
