#pragma once

namespace Graphics::D2D
{
	struct Font
	{
		enum class HAlign
		{
			Left, Center, Right
		};

		enum class VAlign
		{
			Top, Center, Bottom
		};

		bool operator<(const Font& rhs) const
		{
			if (fontName != rhs.fontName)
				return fontName < rhs.fontName;
			if (fontSize != rhs.fontSize)
				return fontSize < rhs.fontSize;
			if (hAlign != rhs.hAlign)
				return hAlign < rhs.hAlign;
			return vAlign < rhs.vAlign;
		}

		std::wstring_view fontName;
		float fontSize;
		HAlign hAlign;
		VAlign vAlign;
	};
}
