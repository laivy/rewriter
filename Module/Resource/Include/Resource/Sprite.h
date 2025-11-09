#pragma once

namespace Resource
{
	struct Sprite
	{
		ComPtr<IUnknown> bitmap; // ID2D1Bitmap;
		std::uint32_t width{ 0 };
		std::uint32_t height{ 0 };
#ifdef _TOOL
		std::vector<char> binary;
#endif
	};
}
