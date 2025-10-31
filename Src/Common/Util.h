#pragma once

namespace Util
{
	std::string ToU8String(std::wstring_view wstr);
	std::wstring ToWString(std::string_view str);
}
