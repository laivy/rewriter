#include "Stdafx.h"
#include "UI.h"
#include "Wnd.h"

UI::UI() : m_parent{ nullptr }
{

}

void UI::SetParent(Wnd* wnd)
{
	m_parent = wnd;
}

Wnd* const UI::GetParent() const
{
	return m_parent;
}

RECTF UI::GetRect() const
{
	FLOAT2 pos{ m_position };
	if (m_parent)
		pos += m_parent->GetPosition();
	return RECTF{ pos.x, pos.y, pos.x + m_size.x, pos.y + m_size.y };
}
