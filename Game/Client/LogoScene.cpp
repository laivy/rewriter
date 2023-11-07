#include "Stdafx.h"
#include "LoginScene.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "ClientApp.h"
#include "SceneManager.h"

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

void LogoScene::Render2D() const { }
