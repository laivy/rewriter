#include "Stdafx.h"
#include "TextBlock.h"

TextBlock::TextBlock(IWindow* owner) :
	IControl{ owner }
{
}

void TextBlock::Render() const
{
	Graphics::D2D::DrawText(m_text, {}, {}, m_position);
}

void TextBlock::SetText(std::wstring_view text)
{
	m_text = text;
}
