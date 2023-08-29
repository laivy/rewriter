#include "Stdafx.h"
#include "Button.h"
#include "Wnd.h"

Button::Button(const INT2& size) :
	m_isMouseOver{ false },
	m_isMouseDown{ false },
	m_color{ D2D1::ColorF::Aqua },
	m_id{ 0 }
{
	SetSize(size);
}

void Button::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		RECTI rect{ 0, 0, m_size.x, m_size.y };
		if (rect.IsContain({ x, y }))
			m_isMouseOver = true;
		else
			m_isMouseOver = false;
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (!m_isMouseOver)
			break;

		m_isMouseDown = true;
		break;
	}
	case WM_LBUTTONUP:
	{
		m_isMouseDown = false;
		if (!m_isMouseOver)
			break;

		RECTI rect{ 0, 0, m_size.x, m_size.y };
		if (m_parent && rect.IsContain({ x, y }))
			m_parent->OnButtonClick(m_id);
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

	INT2 position{ m_position };
	position += m_parent->GetPosition(Pivot::LEFTTOP);

	ComPtr<ID2D1SolidColorBrush> brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ m_color }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(static_cast<float>(position.x), static_cast<float>(position.y)));
	renderTarget->FillRectangle(D2D1::RectF(-m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f), brush.Get());
}

void Button::SetButtonID(ButtonID id)
{
	m_id = id;
}
