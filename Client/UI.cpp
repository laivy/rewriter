#include "Stdafx.h"
#include "UI.h"
#include "Wnd.h"

IUserInterface::IUserInterface() :
	m_parent{ nullptr },
	m_isValid{ TRUE },
	m_isFocus{ FALSE },
	m_pivot{ Pivot::CENTER }
{

}

void IUserInterface::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y) { }
void IUserInterface::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void IUserInterface::Update(FLOAT deltaTime) { }
void IUserInterface::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }

void IUserInterface::Destroy()
{
	m_isValid = TRUE;
}

void IUserInterface::SetParent(Wnd* const wnd)
{
	m_parent = wnd;
}

void IUserInterface::SetFocus(bool focus)
{
	m_isFocus = focus;
}

void IUserInterface::SetPivot(Pivot pivot)
{
	m_pivot = pivot;
}

void IUserInterface::SetSize(const INT2& size)
{
	m_size = size;
}

void IUserInterface::SetPosition(const FLOAT2& position)
{
	m_position = position;

	// 기본 피봇인 중심 기준 위치로 설정함
	switch (m_pivot)
	{
	case Pivot::LEFTTOP:
		m_position.x += m_size.x / 2.0f;
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::CENTERTOP:
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::RIGHTTOP:
		m_position.x -= m_size.x / 2.0f;
		m_position.y -= m_size.y / 2.0f;
		break;
	case Pivot::LEFTCENTER:
		m_position.x += m_size.x / 2.0f;
		break;
	case Pivot::CENTER:
		break;
	case Pivot::RIGHTCENTER:
		m_position.x -= m_size.x / 2.0f;
		break;
	case Pivot::LEFTBOT:
		m_position.x += m_size.x / 2.0f;
		m_position.y += m_size.y / 2.0f;
		break;
	case Pivot::CENTERBOT:
		m_position.y += m_size.y / 2.0f;
		break;
	case Pivot::RIGHTBOT:
		m_position.x -= m_size.x / 2.0f;
		m_position.y += m_size.y / 2.0f;
		break;
	}
}

bool IUserInterface::IsValid() const
{
	return m_isValid;
}

bool IUserInterface::IsFocus() const
{
	return m_isFocus;
}

Wnd* const IUserInterface::GetParent() const
{
	return m_parent;
}

RECTF IUserInterface::GetRect() const
{
	FLOAT2 pos{ m_position };
	if (m_parent)
		pos += m_parent->GetPosition();
	return RECTF{ pos.x, pos.y, pos.x + m_size.x, pos.y + m_size.y };
}

INT2 IUserInterface::GetSize() const
{
	return m_size;
}

FLOAT2 IUserInterface::GetPosition() const
{
	return m_position;
}
