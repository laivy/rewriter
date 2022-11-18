#include "Stdafx.h"
#include "Button.h"
#include "Wnd.h"

Button::Button(FLOAT width, FLOAT height, FLOAT x, FLOAT y)
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });
}

void Button::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{
	
}

void Button::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	Wnd* parentWnd{ GetParent() };
	if (!parentWnd) return;

	FLOAT2 position{ m_position };
	position += parentWnd->GetPosition();

	ID2D1SolidColorBrush* brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Aqua }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(position.x, position.y));
	renderTarget->FillRectangle(
		D2D1::RectF(-m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f),
		brush
	);
	brush->Release();
}

void Button::SetPosition(const FLOAT2& position)
{
	m_position = position;
}

RECTF Button::GetRect() const
{
	Wnd* parentWnd{ GetParent() };
	if (!parentWnd)
		return RECTF{};

	FLOAT2 position{ m_position };
	position += parentWnd->GetPosition();

	RECTF rect{};
	rect.top = position.y - m_size.y / 2.0f;
	rect.bottom = position.y + m_size.y / 2.0f;
	rect.left = position.x - m_size.x / 2.0f;
	rect.right = position.x + m_size.x / 2.0f;
	return rect;
}

FLOAT2 Button::GetPosition() const
{
	return m_position;
}
