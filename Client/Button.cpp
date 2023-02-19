#include "Stdafx.h"
#include "Button.h"
#include "Wnd.h"

Button::Button(FLOAT width, FLOAT height, FLOAT x, FLOAT y) : 
	m_color{ D2D1::ColorF::Aqua },
	m_isMouseOver{ FALSE },
	m_isMouseDown{ FALSE }
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
		FLOAT2 pos{ static_cast<FLOAT>(x), static_cast<FLOAT>(y) };
		RECTF rect{ 0.0f, 0.0f, m_size.x, m_size.y };
		rect.Offset(m_position.x, m_position.y);

		if (Util::IsContain(rect, pos))
			m_isMouseOver = TRUE;
		else
			m_isMouseOver = FALSE;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (!m_isMouseOver)
			break;

		m_isMouseDown = TRUE;
		break;
	}
	case WM_LBUTTONUP:
	{
		if (!m_isMouseOver || !m_isMouseDown)
			break;

		m_isMouseDown = FALSE;

		FLOAT2 pos{ static_cast<FLOAT>(x), static_cast<FLOAT>(y) };
		RECTF rect{ 0.0f, 0.0f, m_size.x, m_size.y };
		rect.Offset(m_position.x, m_position.y);

		if (Util::IsContain(rect, pos))
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

void Button::Update(FLOAT deltaTime)
{
	if (m_isMouseOver)
	{
		if (m_isMouseDown)
			m_color = D2D1::ColorF::Blue; // 버튼 클릭
		else
			m_color = D2D1::ColorF::Red; // 마우스오버
	}
	else
	{
		m_color = D2D1::ColorF::Green; // 평소
	}
}

void Button::Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const
{
	if (!m_parent) return;

	FLOAT2 position{ m_position };
	position += m_parent->GetPosition();

	ComPtr<ID2D1SolidColorBrush> brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ m_color }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(position.x, position.y));
	renderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, m_size.y), brush.Get());
}