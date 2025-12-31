#include "Pch.h"
#include "Util.h"

namespace Util
{
	std::string ToU8String(std::wstring_view wstr)
	{
		if (wstr.empty())
			return std::string{};

		const int length{ ::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		if (!::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL))
			assert(false);
		return str;
	}

	std::wstring Util::ToU16String(std::string_view str)
	{
		if (str.empty())
			return std::wstring{};

		const int length{ ::MultiByteToWideChar(CP_UTF8, NULL, str.data(), static_cast<int>(str.size()), NULL, NULL) };
		std::wstring wstr(length, L'\0');
		if (!::MultiByteToWideChar(CP_UTF8, NULL, str.data(), static_cast<int>(str.size()), wstr.data(), length))
			assert(false);
		return wstr;
	}
}
