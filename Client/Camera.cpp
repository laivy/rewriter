#include "Stdafx.h"
#include "Camera.h"
#include "NytApp.h"

Camera::Camera() : m_eye{ 0.0f, 0.0f, 0.0f }, m_at{ 0.0f, 0.0f, 1.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_scale{ 1.0f, 1.0f }, m_degree{ 0.0f }
{
	auto [width, height] { NytApp::GetInstance()->GetWindowSize() };

	m_cbCamera.Init();
	m_cbCamera->viewMatrix = DirectX::XMMatrixIdentity();
	m_cbCamera->projMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f));
}

void Camera::Update(FLOAT deltaTime)
{
	auto eye{ DirectX::XMLoadFloat3(&m_eye) };
	auto at{ DirectX::XMVectorSet(m_eye.x + m_at.x, m_eye.y + m_at.y, m_eye.z + m_at.z, 1.0f) };
	auto up{ DirectX::XMLoadFloat3(&m_up) };
	m_cbCamera->viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up));
	NytApp::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(RootParamIndex::CAMERA, m_cbCamera.GetGPUVirtualAddress());
}

void Camera::SetScale(const FLOAT2& scale)
{
	m_scale = scale;
}

void Camera::SetRotation(FLOAT degree)
{
	m_degree = degree;
}

FLOAT2 Camera::GetScale() const
{
	return m_scale;
}

FLOAT Camera::GetRotation() const
{
	return m_degree;
}

FLOAT2 Camera::GetPosition() const
{
	return FLOAT2{ m_eye.x, m_eye.y };
}

MATRIX Camera::GetMatrix() const
{
	return MATRIX::Identity();
}
