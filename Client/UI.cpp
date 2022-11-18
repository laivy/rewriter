#include "Stdafx.h"
#include "UI.h"
#include "Wnd.h"

UI::UI() :
	m_size{ 0.0f, 0.0f },
	m_parent{ nullptr }
{

}

void UI::SetSize(const FLOAT2& size)
{
	m_size = size;
}

void UI::SetParent(Wnd* wnd)
{
	m_parent = wnd;
}

Wnd* const UI::GetParent() const
{
	return m_parent;
}

FLOAT2 UI::GetSize() const
{
	return m_size;
}

RECTF UI::GetRect() const
{
	FLOAT2 pos{ m_position };
	if (m_parent)
		pos += m_parent->GetPosition();
	FLOAT hx{ m_size.x / 2.0f };
	FLOAT hy{ m_size.y / 2.0f };
	return RECTF{ pos.x - hx, pos.y - hy, pos.x + hx, pos.y + hy };
}
