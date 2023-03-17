#include "Stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player()
{
	m_cbGameObject.Init();
	m_cbGameObject->layer = static_cast<float>(Layer::LOCALPLAYER) / static_cast<float>(Layer::COUNT);
	m_cbGameObject->alpha = 1.0f;
	m_cbGameObject->isFliped = TRUE;
	
	m_mesh = ResourceManager::GetInstance()->GetMesh(Mesh::DEFAULT);
	m_shader = ResourceManager::GetInstance()->GetShader(Shader::DEFAULT);

	auto prop{ ResourceManager::GetInstance()->Load("Main.nyt") };
	m_image = prop->Get<NytImage>("UIStatus/background");
}

void Player::Update(FLOAT deltaTime)
{
	static float degree = 0.0f;
	SetRotation(degree += deltaTime);
	m_cbGameObject->worldMatrix = GetWorldMatrix();
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	if (m_cbGameObject.IsValid())
		commandList->SetGraphicsRootConstantBufferView(RootParamIndex::GAMEOBJECT, m_cbGameObject.GetGPUVirtualAddress());
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_image)
		m_image->UpdateShaderVariable(commandList);
	if (m_mesh)
		m_mesh->Render(commandList);
}
