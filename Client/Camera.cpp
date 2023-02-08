#include "Stdafx.h"
#include "Camera.h"
#include "NytApp.h"

Camera::Camera() : m_scale{ 1.0f, 1.0f }, m_degree{ 0.0f }, m_position{ 0.0f, 0.0f }
{

}

void Camera::Update(FLOAT deltaTime)
{

}

void Camera::SetScale(const FLOAT2& scale)
{
	m_scale = scale;
}

void Camera::SetRotation(FLOAT degree)
{
	m_degree = degree;
}

void Camera::SetPosition(const FLOAT2& position)
{
	m_position = position;
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
	return m_position;
}

MATRIX Camera::GetMatrix() const
{
	auto [width, height] { NytApp::GetInstance()->GetSize() };
	MATRIX scale{ MATRIX::Scale(m_scale, m_position) };
	MATRIX rotate{ MATRIX::Rotation(m_degree, m_position) };
	MATRIX translate{ MATRIX::Translation((width / 2.0f) - m_position.x, (height / 2.0f) - m_position.y) };
	return MATRIX::Identity() * scale * rotate * translate;
}
