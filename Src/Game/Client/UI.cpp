#include "Stdafx.h"
#include "UI.h"
#include "Window.h"

IUserInterface::IUserInterface() :
	m_isFocus{ false },
	m_size{ 0, 0 },
	m_position{ 0, 0 }
{
}

void IUserInterface::Update(float deltaTime)
{
}

void IUserInterface::Render() const
{
}

void IUserInterface::OnMouseEnter(int x, int y)
{
}

void IUserInterface::OnMouseLeave(int x, int y)
{
}

void IUserInterface::OnMouseEvent(UINT message, int x, int y)
{
}

void IUserInterface::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
}

void IUserInterface::SetFocus(bool focus)
{
	m_isFocus = focus;
}

void IUserInterface::SetPosition(const Int2& position, Pivot pivot)
{
	m_position = position;

	// 기본 피봇인 중심 기준 위치로 설정함
	switch (pivot)
	{
	case Pivot::LeftTop:
		break;
	case Pivot::CenterTop:
		m_position.x -= m_size.x / 2;
		break;
	case Pivot::RightTop:
		m_position.x -= m_size.x;
		break;
	case Pivot::LeftCenter:
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::Center:
		m_position.x -= m_size.x / 2;
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::RightCenter:
		m_position.x -= m_size.x;
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::LeftBot:
		m_position.y -= m_size.y;
		break;
	case Pivot::CenterBot:
		m_position.x -= m_size.x / 2;
		m_position.y -= m_size.y;
		break;
	case Pivot::RightBot:
		m_position.x -= m_size.x;
		m_position.y -= m_size.y;
		break;
	}
}

void IUserInterface::SetSize(const Int2& size)
{
	m_size = size;
}

bool IUserInterface::IsFocus() const
{
	return m_isFocus;
}

Int2 IUserInterface::GetPosition(Pivot pivot) const
{
	Int2 position{ m_position };
	switch (pivot)
	{
	case Pivot::LeftTop:
		break;
	case Pivot::CenterTop:
		position.x += m_size.x / 2;
		break;
	case Pivot::RightTop:
		position.x += m_size.x;
		break;
	case Pivot::LeftCenter:
		position.y += m_size.y / 2;
		break;
	case Pivot::Center:
		position.x += m_size.x / 2;
		position.y += m_size.y / 2;
		break;
	case Pivot::RightCenter:
		position.x += m_size.x;
		position.y += m_size.y / 2;
		break;
	case Pivot::LeftBot:
		position.y += m_size.y;
		break;
	case Pivot::CenterBot:
		position.x += m_size.x / 2;
		position.y += m_size.y;
		break;
	case Pivot::RightBot:
		position.x += m_size.x;
		position.y += m_size.y;
		break;
	}
	return position;
}

Int2 IUserInterface::GetSize() const
{
	return m_size;
}

bool IUserInterface::Contains(const Int2& point) const
{
	// point는 부모 좌표계 기준의 좌표
	Rect rect{ 0, 0, m_size.x, m_size.y };
	rect.Offset(m_position);
	return rect.Contains(point);
}
