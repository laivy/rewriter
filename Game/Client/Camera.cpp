#include "Stdafx.h"
#include "App.h"
#include "Camera.h"

Camera::Camera() :
	m_eye{ 0.0f, 0.0f },
	m_at{ 0.0f, 0.0f },
	m_up{ 0.0f, 1.0f },
	m_scale{ 1.0f, 1.0f },
	m_degree{ 0.0f }
{
	const auto& [width, height] { App::size };
	m_cbCamera.Alloc();
	m_cbCamera->viewMatrix = DirectX::XMMatrixIdentity();
	m_cbCamera->projMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f));
}

void Camera::Update(float deltaTime)
{
	auto eye{ DirectX::XMVectorSet(m_eye.x, m_eye.y, 0.0f, 0.0f) };
	auto at{ DirectX::XMVectorSet(m_eye.x + m_at.x, m_eye.y + m_at.y, 1.0f, 1.0f) };
	auto up{ DirectX::XMVectorSet(m_up.x, m_up.y, 0.0f, 0.0f) };
	m_cbCamera->viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eye, at, up));
}

void Camera::SetShaderVariable()
{
	m_cbCamera.SetShaderVariable();
}

void Camera::SetScale(const FLOAT2& scale)
{
	m_scale = scale;

	const auto& [width, height] { App::size };
	m_cbCamera->projMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(static_cast<float>(width / m_scale.x), static_cast<float>(height / m_scale.y), 0.0f, 1.0f));
}

void Camera::SetRotation(float degree)
{
	m_degree = degree;

	auto up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
	auto rotate{ DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(degree)) };
	up = DirectX::XMVector3Transform(up, rotate);
	m_up.x = DirectX::XMVectorGetX(up);
	m_up.y = DirectX::XMVectorGetY(up);
}

void Camera::OnResize(int width, int height)
{
}
