#include "Stdafx.h"
#include "Util.h"
#include <cuchar>

namespace Util
{
	std::string u8stombs(const std::u8string& u8str)
	{
		// u8 -> ws
		int length{ ::MultiByteToWideChar(CP_UTF8, NULL, reinterpret_cast<const char*>(u8str.data()), static_cast<int>(u8str.size()), NULL, NULL) };
		std::wstring wstr(length, '\0');
		::MultiByteToWideChar(CP_UTF8, NULL, reinterpret_cast<const char*>(u8str.data()), static_cast<int>(u8str.size()), wstr.data(), length);

		// ws -> mbs
		length = ::WideCharToMultiByte(CP_ACP, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL);
		std::string str(length, '\0');
		::WideCharToMultiByte(CP_ACP, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL);
		return str;
	}

	std::string wstou8s(const std::wstring& wstr)
	{
		int length{ ::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL);
		return str;
	}
}