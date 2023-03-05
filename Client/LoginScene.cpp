#include "Stdafx.h"
#include "Camera.h"
#include "LoginScene.h"
#include "Mesh.h"
#include "NytLoader.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "NytUI.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "WndManager.h"

LoginScene::LoginScene()
{

}

void LoginScene::OnCreate()
{
	ResourceManager::GetInstance()->AddShader(Shader::Type::DEFAULT, new Shader);
	ResourceManager::GetInstance()->AddMesh(Mesh::Type::DEFAULT, new Mesh);
	m_camera = std::make_unique<Camera>();
	m_player = std::make_unique<Player>();
}

void LoginScene::OnDestory()
{
	NytLoader::GetInstance()->Unload("Login.nyt");
}

void LoginScene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WndManager::GetInstance()->OnMouseEvent(hWnd, message, wParam, lParam);
}

void LoginScene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WndManager::GetInstance()->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void LoginScene::Update(FLOAT deltaTime)
{
	WndManager::GetInstance()->Update(deltaTime);
	if (m_camera)
		m_camera->Update(deltaTime);
	if (m_player)
		m_player->Update(deltaTime);
}

void LoginScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_camera)
		m_camera->UpdateShaderVariable(commandList);
	if (m_player)
		m_player->Render(commandList);
}

void LoginScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	WndManager::GetInstance()->Render(d2dContext);
}