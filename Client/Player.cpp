#include "Stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player() : m_stand{ nullptr }, m_frame{ 0.0f }
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = static_cast<float>(Layer::LOCALPLAYER) / static_cast<float>(Layer::COUNT);
	m_cbGameObject->alpha = 1.0f;
	m_cbGameObject->isFliped = FALSE;
	
	DirectX::XMFLOAT4X4 worldMatrix{};
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	m_cbGameObject->worldMatrix = worldMatrix;
	
	auto rm{ ResourceManager::GetInstance() };
	m_mesh = rm->GetMesh(Mesh::DEFAULT);
	m_shader = rm->GetShader(Shader::DEFAULT);

	auto prop{ rm->Load("Player.nyt") };
	m_stand = prop->Get<NytProperty>("Stand");
}

void Player::Update(FLOAT deltaTime)
{
	if (!m_stand)
		return;

	m_frame += deltaTime * 5.0f;
	if (m_frame > static_cast<FLOAT>(m_stand->GetChildCount()))
		m_frame = 0.0f;
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	if (m_cbGameObject.IsValid())
		commandList->SetGraphicsRootConstantBufferView(RootParamIndex::GAMEOBJECT, m_cbGameObject.GetGPUVirtualAddress());
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_stand)
	{
		auto img{ m_stand->Get<NytImage>(std::to_string(static_cast<INT>(m_frame))) };
		img->UpdateShaderVariable(commandList);
	}
	if (m_mesh)
		m_mesh->Render(commandList);
}
