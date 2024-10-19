#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Window.h"

Button::Button(IWindow* owner) :
	IControl{ owner },
	m_state{ State::Enable }
{
}

void Button::OnMouseEnter(int x, int y)
{
	m_state = State::MouseOver;
}

void Button::OnMouseLeave(int x, int y)
{
	m_state = State::Enable;
}

void Button::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_state = State::MouseDown;
		break;
	}
	case WM_LBUTTONUP:
	{
		OnButtonClick.Notify();
		m_state = State::MouseOver;
		break;
	}
	default:
		break;
	}
}

void Button::Render() const
{
	RECTF rect{ 0.0f, 0.0f, static_cast<float>(m_size.x), static_cast<float>(m_size.y) };
	rect.Offset(m_position);

	switch (m_state)
	{
	case State::Enable:
		Graphics::D2D::DrawRect(rect, D2D1::ColorF::Blue);
		break;
	case State::MouseOver:
		Graphics::D2D::DrawRect(rect, D2D1::ColorF::LightSkyBlue);
		break;
	case State::MouseDown:
		Graphics::D2D::DrawRect(rect, D2D1::ColorF::DarkBlue);
		break;
	}
}
