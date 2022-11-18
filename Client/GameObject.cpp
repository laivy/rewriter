#include "Stdafx.h"
#include "GameObject.h"

INT GameObject::s_id{ 0 };

GameObject::GameObject() :
	m_id{ s_id++ },
	m_position{ 0.0f, 0.0f },
	m_isValid{ TRUE }
{
	
}

void GameObject::Update(FLOAT deltaTime) { }
void GameObject::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const { }

void GameObject::Destroy()
{
	m_isValid = FALSE;
}

void GameObject::SetPosition(const FLOAT2& position)
{
	m_position = position;
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
