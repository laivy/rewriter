#include "Stdafx.h"
#include "LogoScene.h"
#include "Mesh.h"
#include "NytApp.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"

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

}

void LogoScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	auto size{ NytApp::GetInstance()->GetWindowSize() };

	auto logo{ m_prop->Get<NytUI>("ui") };
	logo->Render(d2dContext, size.x / 2.0f, size.y / 2.0f);
}
