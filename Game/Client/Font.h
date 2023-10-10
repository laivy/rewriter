#pragma once

class Font
{
public:
	enum class Type
	{
		MORRIS12
	};

public:
	Font(const std::string& fontPath, float size, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL);
	~Font() = default;

	ComPtr<IDWriteTextFormat> GetTextFormat() const;

private:
	ComPtr<IDWriteTextFormat> m_textFormat;
};