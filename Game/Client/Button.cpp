#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Renderer2D.h"
#include "Window.h"

Button::Button(IWindow* owner) :
	IControl{ owner },
	m_state{ false }
{
	m_textFormat = Renderer2D::CreateTextFormat(L"", 16);
}

void Button::OnMouseEnter(int x, int y)
{
	m_state = State::MOUSEOVER;
}

void Button::OnMouseLeave(int x, int y)
{
	m_state = State::DEFAULT;
}

void Button::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_state = State::MOUSEDOWN;
		break;
	}
	case WM_LBUTTONUP:
	{
		OnButtonClick.Notify();
		m_state = State::MOUSEOVER;
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
	case State::DEFAULT:
		Renderer2D::DrawRect(rect, D2D1::ColorF::Blue);
		break;
	case State::MOUSEOVER:
		Renderer2D::DrawRect(rect, D2D1::ColorF::LightSkyBlue);
		break;
	case State::MOUSEDOWN:
		Renderer2D::DrawRect(rect, D2D1::ColorF::DarkBlue);
		break;
	}

	if (m_textLayout)
		Renderer2D::DrawText(m_position, m_textLayout, D2D1::ColorF::Black);
}

void Button::SetText(std::wstring_view text)
{
	m_text = text;
	m_textLayout = Renderer2D::CreateTextLayout(m_text, m_textFormat, m_size.x, m_size.y);
	m_textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}
