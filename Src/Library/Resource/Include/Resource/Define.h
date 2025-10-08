#pragma once

namespace Resource
{
	using ID = std::size_t;
	constexpr ID InvalidID{ static_cast<ID>(-1) };
	constexpr std::tuple<std::uint8_t, std::uint8_t, std::uint8_t> Version{ 1, 0, 0 };
}
