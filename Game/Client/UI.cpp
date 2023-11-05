#include "Stdafx.h"
#include "UI.h"
#include "Wnd.h"

IUserInterface::IUserInterface() :
	m_parent{ nullptr },
	m_isValid{ true },
	m_isFocus{ false },
	m_pivot{ Pivot::LEFTTOP },
	m_size{ 0, 0 },
	m_position{ 0, 0 }
{
}

void IUserInterface::OnMouseMove(int x, int y) { }
void IUserInterface::OnLButtonUp(int x, int y) { }
void IUserInterface::OnLButtonDown(int x, int y) { }
void IUserInterface::OnRButtonUp(int x, int y) { }
void IUserInterface::OnRButtonDown(int x, int y) { }
void IUserInterface::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) { }
void IUserInterface::Update(float deltaTime) { }
void IUserInterface::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }

void IUserInterface::Destroy()
{
	m_isValid = true;
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

void IUserInterface::SetPosition(const INT2& position)
{
	m_position = position;

	// 기본 피봇인 중심 기준 위치로 설정함
	switch (m_pivot)
	{
	case Pivot::LEFTTOP:
		m_position.x += m_size.x / 2;
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::CENTERTOP:
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::RIGHTTOP:
		m_position.x -= m_size.x / 2;
		m_position.y -= m_size.y / 2;
		break;
	case Pivot::LEFTCENTER:
		m_position.x += m_size.x / 2;
		break;
	case Pivot::CENTER:
		break;
	case Pivot::RIGHTCENTER:
		m_position.x -= m_size.x / 2;
		break;
	case Pivot::LEFTBOT:
		m_position.x += m_size.x / 2;
		m_position.y += m_size.y / 2;
		break;
	case Pivot::CENTERBOT:
		m_position.y += m_size.y / 2;
		break;
	case Pivot::RIGHTBOT:
		m_position.x -= m_size.x / 2;
		m_position.y += m_size.y / 2;
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

bool IUserInterface::IsContain(const INT2& point) const
{
	// point는 부모 좌표계 기준의 좌표
	RECTI rect{ 0, 0, m_size.x, m_size.y };
	rect.Offset(m_position.x, m_position.y);
	return rect.IsContain(point);
}

Wnd* const IUserInterface::GetParent() const
{
	return m_parent;
}

INT2 IUserInterface::GetSize() const
{
	return m_size;
}

INT2 IUserInterface::GetPosition(Pivot pivot) const
{
	INT2 position{ m_position };
	switch (pivot)
	{
	case Pivot::LEFTTOP:
		position.x -= m_size.x / 2;
		position.y -= m_size.y / 2;
		break;
	case Pivot::CENTERTOP:
		position.y -= m_size.y / 2;
		break;
	case Pivot::RIGHTTOP:
		position.x += m_size.x / 2;
		position.y -= m_size.y / 2;
		break;
	case Pivot::LEFTCENTER:
		position.x -= m_size.x / 2;
		break;
	case Pivot::CENTER:
		break;
	case Pivot::RIGHTCENTER:
		position.x += m_size.x / 2;
		break;
	case Pivot::LEFTBOT:
		position.x -= m_size.x / 2;
		position.y += m_size.y / 2;
		break;
	case Pivot::CENTERBOT:
		position.y += m_size.y / 2;
		break;
	case Pivot::RIGHTBOT:
		position.x += m_size.x / 2;
		position.y += m_size.y / 2;
		break;
	}
	return position;
}
