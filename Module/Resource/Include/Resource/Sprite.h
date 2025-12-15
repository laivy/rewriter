#pragma once
#include <cstdint>
#include <memory>
#include <vector>

namespace Resource
{
	struct Sprite
	{
#ifdef _TOOL
		std::vector<std::byte> binary;
#endif
		std::uint32_t width{ 0 };
		std::uint32_t height{ 0 };
	};
}
