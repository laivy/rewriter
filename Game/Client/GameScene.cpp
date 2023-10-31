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
	auto om{ ObjectManager::GetInstance() };

	auto player{ std::make_shared<LocalPlayer>() };
	player->SetPosition({ 960.0f, 500.0f });
	om->SetLocalPlayer(player);

	auto camera{ std::make_shared<FocusCamera>() };
	camera->SetPosition(player->GetPosition());
	camera->SetFocus(player);
	om->SetCamera(camera);

	auto map = std::make_shared<Map>();
	map->m_platforms.emplace_back(std::make_shared<Platform>(INT2{ 0, 100 }, INT2{ 1920, 100 }));
	map->m_platforms.emplace_back(std::make_shared<Platform>(INT2{ 930, 200 }, INT2{ 990, 200 }));
	map->m_platforms.emplace_back(std::make_shared<Platform>(INT2{ 930, 300 }, INT2{ 990, 300 }));
	map->m_platforms.emplace_back(std::make_shared<Platform>(INT2{ 930, 400 }, INT2{ 990, 400 }));
	map->m_platforms.emplace_back(std::make_shared<Platform>(INT2{ 930, 500 }, INT2{ 990, 500 }));
	om->SetMap(map);
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
	if (auto om{ ObjectManager::GetInstance() })
		om->Update(deltaTime);
	if (auto wm{ WndManager::GetInstance() })
		wm->Update(deltaTime);
}

void GameScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (auto om{ ObjectManager::GetInstance() })
		om->Render(commandList);
}

void GameScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (auto wm{ WndManager::GetInstance() })
		wm->Render(d2dContext);
}