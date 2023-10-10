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

void Button::OnMouseMove(int x, int y)
{
	RECTI rect{ 0, 0, m_size.x, m_size.y };
	if (rect.IsContain({ x, y }))
		m_isMouseOver = true;
	else
		m_isMouseOver = false;
}

void Button::OnLButtonUp(int x, int y)
{
	m_isMouseDown = false;
	if (!m_isMouseOver)
		return;

	RECTI rect{ 0, 0, m_size.x, m_size.y };
	if (m_parent && rect.IsContain({ x, y }))
		m_parent->OnButtonClick(m_id);
}

void Button::OnLButtonDown(int x, int y)
{
	if (m_isMouseOver)
		m_isMouseDown = true;
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

void Button::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (!m_parent) return;

	INT2 position{ m_position };
	position += m_parent->GetPosition(Pivot::LEFTTOP);

	ComPtr<ID2D1SolidColorBrush> brush{};
	d2dContext->CreateSolidColorBrush(D2D1::ColorF{ m_color }, &brush);
	d2dContext->SetTransform(D2D1::Matrix3x2F::Translation(static_cast<float>(position.x), static_cast<float>(position.y)));
	d2dContext->FillRectangle(RECTF{ -m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f }, brush.Get());
}

void Button::SetButtonID(ButtonID id)
{
	m_id = id;
}
