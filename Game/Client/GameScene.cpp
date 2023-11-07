#include "Stdafx.h"
#include "Camera.h"
#include "GameScene.h"
#include "Map.h"
#include "ObjectManager.h"
#include "Platform.h"
#include "Player.h"
#include "UI.h"
#include "WndManager.h"

void GameScene::OnCreate()
{
}

void GameScene::OnDestroy()
{
}

void GameScene::OnResize(int width, int height)
{
}

void GameScene::OnMouseMove(int x, int y)
{
}

void GameScene::OnLButtonDown(int x, int y)
{
}

void GameScene::OnLButtonUp(int x, int y)
{
}

void GameScene::OnRButtonDown(int x, int y)
{
}

void GameScene::OnRButtonUp(int x, int y)
{
}

void GameScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
}

void GameScene::Update(FLOAT deltaTime)
{
	if (auto wm{ WndManager::GetInstance() })
		wm->Update(deltaTime);
}

void GameScene::Render3D(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
}

void GameScene::Render2D() const
{
	if (auto wm{ WndManager::GetInstance() })
		wm->Render();
}