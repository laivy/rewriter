#include "Stdafx.h"
#include "Player.h"
#include "Mesh.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "ResourceManager.h"
#include "Shader.h"

Player::Player() : m_animationComponent{ this }, m_inputComponent{ this }
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
}

void Player::Update(FLOAT deltaTime)
{
	m_animationComponent.Update(deltaTime);
	m_inputComponent.Update(deltaTime);
}

void Player::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_animationComponent.SetShaderVariable(commandList);
	if (m_cbGameObject.IsValid())
		m_cbGameObject.SetShaderVariable(commandList, RootParamIndex::GAMEOBJECT);
	if (m_shader)
		commandList->SetPipelineState(m_shader->GetPipelineState());
	if (m_mesh)
		m_mesh->Render(commandList);
}

Player::AnimationComponent::AnimationComponent(Player* player) : m_player{ player }, m_type { Type::STAND }, m_frame{ 0 }, m_timer{ 0.0f }
{
	auto rm{ ResourceManager::GetInstance() };
	m_root = rm->Load("Player.nyt");
	m_currAniProp = m_root->Get<NytProperty>("Stand");
}

void Player::AnimationComponent::OnAnimationStart()
{

}

void Player::AnimationComponent::OnAnimationEnd()
{
	switch (m_type)
	{
	case Type::STAND:
		PlayAnimation(Type::STAND);
		break;
	case Type::ATTACK1:
		break;
	case Type::ATTACK2:
		break;
	default:
		break;
	}
}

void Player::AnimationComponent::Update(FLOAT deltaTime)
{
	m_timer += deltaTime;
	if (m_timer >= FRAME_INTERVAL)
	{
		m_frame += static_cast<INT>(m_timer / FRAME_INTERVAL);
		m_timer = fmod(m_timer, FRAME_INTERVAL);
	}

	if (m_frame >= m_currAniProp->GetChildCount() - 1)
	{
		m_frame = m_currAniProp->GetChildCount() - 1;
		OnAnimationEnd();
	}
}

void Player::AnimationComponent::PlayAnimation(Type type)
{
	m_type = type;
	m_frame = 0;
	m_timer = 0.0f;
	OnAnimationStart();
}

void Player::AnimationComponent::SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	m_currAniProp->Get<NytImage>(std::to_string(m_frame))->SetShaderVariable(commandList);
}

Player::InputComponent::InputComponent(Player* player) : m_player{ player }
{
}

void Player::InputComponent::Update(FLOAT deltaTime)
{
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_player->Move(FLOAT2{ -SPEED * deltaTime, 0.0f });
	}
}
