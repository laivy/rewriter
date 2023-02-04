#include "Stdafx.h"
#include "Scene.h"
#include "SceneManager.h"
#include "LoginScene.h"
#include "BrushPool.h"

SceneManager::SceneManager() : m_scene{}, m_nextScene{}
{
	LoginScene::Instantiate();
	if (LoginScene::IsInstanced())
	{
		LoginScene::GetInstance()->OnCreate();
		SetScene(LoginScene::GetInstance());
	}
}

void SceneManager::OnDestroy()
{
	if (m_scene)
		m_scene->OnDestory();
}

void SceneManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_scene)
		m_scene->OnMouseEvent(hWnd, message, wParam, lParam);
}

void SceneManager::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_scene)
		m_scene->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void SceneManager::Update(FLOAT deltaTime)
{
	if (m_scene)
		m_scene->Update(deltaTime);
}

void SceneManager::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	if (m_scene)
		m_scene->Render(renderTarget);
}