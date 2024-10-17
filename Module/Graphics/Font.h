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

		std::wstring_view name;
		float size;
		HAlign hAlign;
		VAlign vAlign;
	};
}
