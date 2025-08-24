#include "Stdafx.h"
#include "TextBlock.h"

TextBlock::TextBlock(IWindow* owner, const Resource::Property::ID id) :
	IControl{ owner },
	m_font{ L"", 16.0f }
{
	Build(prop);
}

void TextBlock::Render() const
{
	Graphics::D2D::DrawText(m_text, m_font, Graphics::D2D::Color{}, m_position);
}

void TextBlock::SetText(std::wstring_view text)
{
	if (m_text == text)
		return;

	m_text = text;
	auto metrics{ Graphics::D2D::GetTextMetrics(m_text, m_font) };
	SetSize(Int2{ metrics.left + metrics.width, metrics.top + metrics.height });
}

void TextBlock::Build(const Resource::Property::ID id)
{
	/*
	- Z(Int)
		- 그려질 레이어의 z값
	- Position(Int2)
		- 위치
	- Text(String)
		- 보여줄 텍스트
	- FontName(String)
		- 폰트 이름
	- FontSize(Float)
		- 폰트 크기
	*/

	if (!prop)
		return;

	SetZ(prop->GetInt(L"Z"));
	SetPosition(prop->GetInt2(L"Position"));
	SetText(prop->GetString(L"Text"));
	m_font.name = prop->GetString(L"FontName");
	m_font.size = prop->GetFloat(L"FontSize");
}
