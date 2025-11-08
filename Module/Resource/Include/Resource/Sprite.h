#pragma once

namespace Resource
{
	struct Sprite
	{
		ComPtr<IUnknown> bitmap; // ID2D1Bitmap;
		float width{ 0 };
		float height{ 0 };
#ifdef _TOOL
		std::vector<std::byte> binary;
#endif
	};
}
