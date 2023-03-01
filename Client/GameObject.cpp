#include "Stdafx.h"
#include "GameObject.h"

INT GameObject::s_id{ 0 };

GameObject::GameObject() :
	m_id{ s_id++ },
	m_isValid{ TRUE },
	m_size{ 0.0f, 0.0f },
	m_scale{ 1.0f, 1.0f },
	m_degree{ 0.0f },
	m_position{ 0.0f, 0.0f }
{
	
}

void GameObject::Update(FLOAT deltaTime) { }
void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const { }

void GameObject::Destroy()
{
	m_isValid = FALSE;
}

void GameObject::SetPosition(const FLOAT2& position, Pivot pivot)
{
	m_position = position;
	switch (pivot)
	{
	case Pivot::LEFTTOP:
		break;
	case Pivot::CENTERTOP:
		m_position.x -= m_size.x / 2.0f;
		break;
	case Pivot::RIGHTTOP:
		m_position.x -= m_size.x;
		break;
	case Pivot::LEFTCENTER:
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::CENTER:
		m_position.x -= m_size.x / 2.0f;
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::RIGHTCENTER:
		m_position.x -= m_size.x;
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::LEFTBOT:
		m_position.y -= m_size.y;
		break;
	case Pivot::CENTERBOT:
		m_position.x -= m_size.x / 2.0f;
		m_position.y -= m_size.y;
		break;
	case Pivot::RIGHTBOT:
		m_position.x -= m_size.x;
		m_position.y -= m_size.y;
		break;
	}
}

void GameObject::SetPosition(FLOAT x, FLOAT y, Pivot pivot)
{
	SetPosition(FLOAT2{ x, y }, pivot);
}

void GameObject::SetSize(const FLOAT2& size)
{
	m_size = size;
}

void GameObject::SetScale(const FLOAT2& scale)
{
	m_scale = scale;
}

void GameObject::SetRotation(FLOAT degree)
{
	m_degree = degree;
}

INT GameObject::GetId() const
{
	return m_id;
}

BOOL GameObject::IsValid() const
{
	return m_isValid;
}

FLOAT2 GameObject::GetPosition() const
{
	return m_position;
}

FLOAT2 GameObject::GetSize() const
{
	return m_size;
}

MATRIX GameObject::GetMatrix() const
{
	MATRIX scale{ MATRIX::Scale(m_scale, m_position) };
	MATRIX rotate{ MATRIX::Rotation(m_degree, m_position) };
	MATRIX translate{ MATRIX::Translation(m_position) };
	return MATRIX::Identity() * scale * rotate * translate;
}