#include "Stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "NytImage.h"
#include "NytLoader.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player()
{
	m_cbGameObject.Init();
	m_cbGameObject->worldMatrix = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	m_mesh = ResourceManager::GetInstance()->GetMesh(Mesh::Type::DEFAULT);
	m_shader = ResourceManager::GetInstance()->GetShader(Shader::Type::DEFAULT);

	auto prop{ NytLoader::GetInstance()->Load("Main.nyt") };
	m_image = prop->Get<NytImage>("UIStatus/background");
}

void Player::Update(FLOAT deltaTime)
{

}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	if (m_cbGameObject.IsValid())
		commandList->SetGraphicsRootConstantBufferView(RootParamIndex::GAMEOBJECT, m_cbGameObject.GetGPUVirtualAddress());
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_image)
		m_image->UpdateShaderVariable(commandList, RootParamIndex::TEXTURE0);
	if (m_mesh)
		m_mesh->Render(commandList);
}
