#include "Stdafx.h"
#include "LoginScene.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "NytApp.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "SceneManager.h"

LogoScene::LogoScene() : m_prop{ nullptr }
{

}

LogoScene::~LogoScene()
{
	LogoScene::Destroy();
}

void LogoScene::OnCreate()
{
	auto rm{ ResourceManager::GetInstance() };
	rm->AddMesh(Mesh::DEFAULT, new Mesh);
	
	m_prop = rm->Load("Main.nyt")->Get<NytProperty>("UIStatus");
}

void LogoScene::OnDestory()
{

}

void LogoScene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void LogoScene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void LogoScene::Update(FLOAT deltaTime)
{
	static BOOL isFirstUpdate{ TRUE };
	if (isFirstUpdate)
	{
		SceneManager::GetInstance()->SetFadeOut(1.0f,
			[]()
			{
				LoginScene::Instantiate();
				auto sm{ SceneManager::GetInstance() };
				sm->SetScene(LoginScene::GetInstance());
				sm->SetFadeIn(1.0f);
				LogoScene::Destroy();
			}
		);
		isFirstUpdate = FALSE;
	}
}

void LogoScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	auto size{ NytApp::GetInstance()->GetWindowSize() };

	auto logo{ m_prop->Get<NytUI>("ui") };
	logo->Render(d2dContext, 0.0f, 0.0f);
}
