#include "Stdafx.h"
#include "TextUtil.h"

namespace TextUtil
{
	std::wstring str2wstr(const std::string& str)
	{
		int wstrLength = MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), -1, nullptr, 0);
		if (wstrLength == 0)
			return TEXT("");

		std::wstring wStr;
		wStr.resize(static_cast<size_t>(wstrLength) - 1);

		if (MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wStr.data(), wstrLength) == 0)
			return TEXT("");

		return wStr;
	}
}