#include "Stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"

INT GameObject::s_id{ 0 };

GameObject::GameObject() :
	m_id{ s_id++ },
	m_isValid{ TRUE },
	m_size{ 0.0f, 0.0f },
	m_scale{ 1.0f, 1.0f },
	m_degree{ 0.0f },
	m_position{ 0.0f, 0.0f },
	m_shader{ nullptr },
	m_mesh{ nullptr }
{
	
}

void GameObject::Update(FLOAT deltaTime) { }
void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_mesh)
		m_mesh->Render(commandList);
}

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

void GameObject::SetLayer(Layer layer)
{
	m_layer = layer;
}

void GameObject::SetShader(Shader* shader)
{
	m_shader = shader;
}

void GameObject::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
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

DirectX::XMFLOAT4X4 GameObject::GetWorldMatrix() const
{
	using namespace DirectX;
	XMMATRIX scale{ XMMatrixScaling(m_scale.x, m_scale.y, 1.0f) };
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(m_degree)) };
	XMMATRIX translate{ XMMatrixTranslation(m_position.x, m_position.y, 0.0f) };
	
	XMFLOAT4X4 worldMatrix{};
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(scale * rotate * translate));
	return worldMatrix;
}