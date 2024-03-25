#include "Stdafx.h"
#include "Util.h"
#include <cuchar>

namespace Util
{
	std::string u8tos(const std::u8string& u8str)
	{
		return std::string{ u8str.begin(), u8str.end() };
	}

	std::string wstos(const std::wstring& wstr)
	{
		int length{ ::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL);
		return str;
	}
}