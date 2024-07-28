#pragma once

namespace Util
{
	std::string wstou8s(std::wstring_view wstr);
	std::string wstombs(std::wstring_view wstr);
	std::string u8stou8s(std::u8string_view u8str);
	std::string u8stombs(std::u8string_view u8str);
	std::wstring u8stows(std::string_view u8str);
}