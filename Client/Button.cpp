#include "Stdafx.h"
#include "Button.h"
#include "Wnd.h"

Button::Button(FLOAT width, FLOAT height, FLOAT x, FLOAT y) : m_color{ D2D1::ColorF::Aqua }
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });
}

void Button::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		RECTF rect{ m_position.x, m_position.y, m_position.x + m_size.x, m_position.y + m_size.y };
		if (Util::IsContain(rect, POINT{ x, y }))
			m_color = D2D1::ColorF::Red;
		else
			m_color = D2D1::ColorF::Aqua;
		break;
	}
	case WM_LBUTTONUP:
	{
		RECTF rect{ m_position.x, m_position.y, m_position.x + m_size.x, m_position.y + m_size.y };
		if (Util::IsContain(rect, POINT{ x, y }))
		{
			if (m_parent)
				m_parent->OnButtonClicked(GetId());
		}
		break;
	}
	default:
		break;
	}
}

void Button::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	if (!m_parent) return;

	FLOAT2 position{ m_position };
	position += m_parent->GetPosition();

	ComPtr<ID2D1SolidColorBrush> brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ m_color }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(position.x, position.y));
	renderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, m_size.y), brush.Get());
}