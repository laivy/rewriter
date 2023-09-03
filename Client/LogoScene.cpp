#include "Stdafx.h"
#include "LoginScene.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "GameApp.h"
#include "Image.h"
#include "Property.h"
#include "ResourceManager.h"
#include "SceneManager.h"

LogoScene::LogoScene() : m_prop{ nullptr }
{

}

void LogoScene::OnCreate()
{
	if (auto rm{ ResourceManager::GetInstance() })
		m_prop = rm->Load("Main.nyt")->Get<Property>("UIStatus");

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

void LogoScene::OnLButtonDown(int x, int y)
{

}

void LogoScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) { }

void LogoScene::Update(FLOAT deltaTime) { }

void LogoScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	auto logo{ m_prop->Get<Image>("ui") };
	logo->Render(d2dContext, 0.0f, 0.0f);
}
