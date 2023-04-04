#include "Stdafx.h"
#include "LoginScene.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "NytApp.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "SceneManager.h"

LogoScene::LogoScene() : m_prop{ nullptr }, m_isFirstUpdate{ TRUE }
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
	if (!m_isFirstUpdate)
		return;

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
	m_isFirstUpdate = FALSE;
}

void LogoScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	auto logo{ m_prop->Get<NytImage>("ui") };
	logo->Render(d2dContext, 0.0f, 0.0f);
}
