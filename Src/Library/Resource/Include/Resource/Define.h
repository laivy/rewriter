#pragma once

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

namespace Resource
{
	using ID = std::size_t;
	constexpr ID InvalidID{ static_cast<ID>(-1) };
	constexpr std::tuple Version{ 1, 0, 0 };
}
