#include "Stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"

IGameObject::IGameObject() :
	m_isValid{ TRUE },
	m_layer{ Layer::LOCALPLAYER },
	m_size{ 0.0f, 0.0f },
	m_scale{ 1.0f, 1.0f },
	m_degree{ 0.0f },
	m_position{ 0.0f, 0.0f },
	m_pivot{ Pivot::CENTER },
	m_shader{},
	m_mesh{}
{
	
}

void IGameObject::Update(FLOAT deltaTime) { }

void IGameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const { }

void IGameObject::Destroy()
{
	m_isValid = FALSE;
}

void IGameObject::Move(const FLOAT2& delta)
{
	SetPosition(m_position + delta);
}

void IGameObject::SetLayer(Layer layer)
{
	m_layer = layer;
	if (m_cbGameObject.IsValid())
		m_cbGameObject->layer = static_cast<INT>(layer);
}

void IGameObject::SetPivot(Pivot pivot)
{
	m_pivot = pivot;
}

void IGameObject::SetSize(const FLOAT2& size)
{
	m_size = size;
}

void IGameObject::SetScale(const FLOAT2& scale)
{
	m_scale = scale;
}

void IGameObject::SetRotation(FLOAT degree)
{
	m_degree = degree;
}

void IGameObject::SetPosition(const FLOAT2& position)
{
	m_position = position;
	if (m_cbGameObject.IsValid())
		m_cbGameObject->worldMatrix = GetWorldMatrix();
}

bool IGameObject::IsValid() const
{
	return m_isValid;
}

FLOAT2 IGameObject::GetPosition() const
{
	return m_position;
}

FLOAT2 IGameObject::GetSize() const
{
	return m_size;
}

DirectX::XMFLOAT4X4 IGameObject::GetWorldMatrix() const
{
	// 피봇을 고려하여 위치 설정
	float deltaX{}, deltaY{};
	switch (m_pivot)
	{
	case Pivot::LEFTTOP:
		deltaX = -m_size.x / 2.0f;
		deltaY = -m_size.y / 2.0f;
		break;
	case Pivot::CENTERTOP:
		deltaY = -m_size.y / 2.0f;
		break;
	case Pivot::RIGHTTOP:
		deltaX = m_size.x / 2.0f;
		deltaY = -m_size.y / 2.0f;
		break;
	case Pivot::LEFTCENTER:
		deltaX = -m_size.x / 2.0f;
		break;
	case Pivot::CENTER:
		break;
	case Pivot::RIGHTCENTER:
		deltaX = m_size.x / 2.0f;
		break;
	case Pivot::LEFTBOT:
		deltaX = -m_size.x / 2.0f;
		deltaY = m_size.y / 2.0f;
		break;
	case Pivot::CENTERBOT:
		deltaY = m_size.y / 2.0f;
		break;
	case Pivot::RIGHTBOT:
		deltaX = m_size.x / 2.0f;
		deltaY = m_size.y / 2.0f;
		break;
	}

	using namespace DirectX;
	XMMATRIX scale{ XMMatrixScaling(m_scale.x, m_scale.y, 1.0f) };
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(m_degree)) };
	XMMATRIX translate{ XMMatrixTranslation(m_position.x + deltaX, m_position.y + deltaY, 0.0f) };

	XMFLOAT4X4 worldMatrix{};
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(scale * rotate * translate));
	return worldMatrix;
}