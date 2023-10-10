#pragma once

enum class LayoutChangeType
{
	SIZE, WEIGHT
};

struct LayoutChangeInfo
{
	// 태그 구분
	LayoutChangeType changeType;
	std::wstring startTagName;
	std::wstring endTagName;

	// 변경 정보
	FLOAT fontSize;
	DWRITE_FONT_WEIGHT fontWeight;
	DWRITE_TEXT_RANGE textRange;
};

namespace TextUtil
{
	std::wstring str2wstr(const std::string& str);
}