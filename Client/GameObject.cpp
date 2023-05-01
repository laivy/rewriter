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
	m_pivot{ Pivot::LEFTTOP },
	m_shader{ nullptr },
	m_mesh{ nullptr }
{
	
}

void IGameObject::Update(FLOAT deltaTime) { }

void IGameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_mesh)
		m_mesh->Render(commandList);
}

void IGameObject::Destroy()
{
	m_isValid = FALSE;
}

void IGameObject::Move(const FLOAT2& delta)
{
	SetPosition(m_position + delta, m_pivot);
}

void IGameObject::SetLayer(Layer layer)
{
	m_layer = layer;
	if (m_cbGameObject.IsValid())
		m_cbGameObject->layer = static_cast<float>(layer) / static_cast<float>(Layer::COUNT);
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

void IGameObject::SetPosition(const FLOAT2& position, Pivot pivot)
{
	m_position = position;
	m_pivot = pivot;
	switch (m_pivot)
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

	if (m_cbGameObject.IsValid())
		m_cbGameObject->worldMatrix = GetWorldMatrix();
}

void IGameObject::SetShader(Shader* shader)
{
	m_shader = shader;
}

void IGameObject::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

BOOL IGameObject::IsValid() const
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
	using namespace DirectX;
	XMMATRIX scale{ XMMatrixScaling(m_scale.x, m_scale.y, 1.0f) };
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(m_degree)) };
	XMMATRIX translate{ XMMatrixTranslation(m_position.x, m_position.y, 0.0f) };
	
	XMFLOAT4X4 worldMatrix{};
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(scale * rotate * translate));
	return worldMatrix;
}