#include "Stdafx.h"
#include "Camera.h"
#include "GameApp.h"
#include "GameObject.h"
#include "Map.h"
#include "ObjectManager.h"

Camera::Camera() : 
	IGameObject{},
	m_at{ 0.0f, 0.0f }, 
	m_up{ 0.0f, 1.0f }
{
	const auto& [width, height] { GameApp::GetInstance()->GetWindowSize() };

	m_cbCamera.Init();
	m_cbCamera->viewMatrix = DirectX::XMMatrixIdentity();
	m_cbCamera->projMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f));
}

void Camera::Update(FLOAT deltaTime)
{
	auto eye{ DirectX::XMVectorSet(m_position.x, m_position.y, 0.0f, 0.0f) };
	auto at{ DirectX::XMVectorSet(m_position.x + m_at.x, m_position.y + m_at.y, 1.0f, 1.0f) };
	auto up{ DirectX::XMVectorSet(m_up.x, m_up.y, 0.0f, 0.0f) };
	m_cbCamera->viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up));
}

void Camera::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	m_cbCamera.SetShaderVariable(commandList, RootParamIndex::CAMERA);
}

void Camera::SetScale(const FLOAT2& scale)
{
	m_scale = scale;

	if (m_cbCamera.IsValid())
	{
		const auto& [width, height] { GameApp::GetInstance()->GetWindowSize() };
		m_cbCamera->projMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(width / m_scale.x), static_cast<float>(height / m_scale.y), 0.0f, 1.0f));
	}
}

void Camera::SetRotation(FLOAT degree)
{
	m_degree = degree;

	auto up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
	auto rotate{ DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(degree)) };
	up = DirectX::XMVector3Transform(up, rotate);
	m_up.x = DirectX::XMVectorGetX(up);
	m_up.y = DirectX::XMVectorGetY(up);
}

RECTF Camera::GetCameraBoundary() const
{
	auto map{ ObjectManager::GetInstance()->GetMap().lock() };
	if (!map)
		return RECTF{};

	INT2 mapSize{ map->GetSize() };
	RECTF mapBoundary{ 0.0f, static_cast<float>(mapSize.y), static_cast<float>(mapSize.x), 0.0f };

	const auto& [width, height] { GameApp::GetInstance()->GetWindowSize() };
	FLOAT2 cameraRange{ width / m_scale.x, height / m_scale.y };

	RECTF boundary{};
	boundary.left = mapBoundary.left + cameraRange.x / 2.0f;
	boundary.top = mapBoundary.top - cameraRange.y / 2.0f;
	boundary.right = mapBoundary.right - cameraRange.x / 2.0f;
	boundary.bottom = mapBoundary.bottom + cameraRange.y / 2.0f;

	// 맵이 카메라 범위보다 작을 수도 있음
	if (boundary.left > boundary.right)
		boundary.left = boundary.right = 0.0f;
	if (boundary.top < boundary.bottom)
		boundary.top = boundary.bottom = 0.0f;

	return boundary;
}

FocusCamera::FocusCamera() : 
	Camera{},
	m_focus{},
	m_delay{ 0.75f }
{

}

void FocusCamera::Update(FLOAT deltaTime)
{
	if (auto focus{ m_focus.lock() })
	{
		RECTF boundary{ GetCameraBoundary() };
		FLOAT2 pos{ GetPosition() };
		FLOAT2 delta{ focus->GetPosition() - pos };

		delta.x = std::clamp(delta.x, boundary.left - pos.x, boundary.right - pos.x);
		delta.y = std::clamp(delta.y, boundary.bottom - pos.y, boundary.top - pos.y);

		SetPosition(GetPosition() + delta / m_delay * deltaTime);
	}
	else
	{
		focus.reset();
	}
	Camera::Update(deltaTime);
}

void FocusCamera::SetFocus(const std::weak_ptr<IGameObject>& focus)
{
	m_focus = focus;
}
