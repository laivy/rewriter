#include "stdafx.h"
#include "Renderer2D.h"
#include "TextBlock.h"

TextBlock::TextBlock(IWindow* owner) :
	IControl{ owner }
{
	m_textFormat = Renderer2D::CreateTextFormat(L"", 16);
}

void TextBlock::Render() const
{
	if (m_textFormat)
		Renderer2D::DrawText(m_position, m_textLayout, D2D1::ColorF::Black);
}

void TextBlock::SetText(std::wstring_view text)
{
	m_text = text;
	m_textLayout = Renderer2D::CreateTextLayout(m_text, m_textFormat, m_size.x, m_size.y);
}
