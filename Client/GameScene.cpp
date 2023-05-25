#include "Stdafx.h"
#include "Camera.h"
#include "GameScene.h"
#include "Player.h"
#include "ResourceManager.h"

GameScene::GameScene() : 
	m_camera{ nullptr }, 
	m_player{ nullptr }
{

}

GameScene::~GameScene()
{

}

void GameScene::OnCreate()
{
	m_camera = std::make_unique<Camera>();
	m_player = std::make_unique<Player>();
	m_player->SetPosition(FLOAT2{ 100.0f, 0.0f }, Pivot::CENTERBOT);
}

void GameScene::OnDestory()
{
}

void GameScene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

void GameScene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

void GameScene::Update(FLOAT deltaTime)
{
	if (m_camera)
		m_camera->Update(deltaTime);
	if (m_player)
		m_player->Update(deltaTime);
}

void GameScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_camera)
		m_camera->SetShaderVariable(commandList);
	if (m_player)
		m_player->Render(commandList);
}

void GameScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{

}