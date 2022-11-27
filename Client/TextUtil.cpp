#include "Stdafx.h"
#include "TextUtil.h"

namespace TextUtil
{
	// 텍스트에 있는 태그들을 LayoutChangeInfo로 만들어서 반환한다.
	std::vector<LayoutChangeInfo> TextUtil::ApplyTextTag(std::wstring& text)
	{
		std::vector<LayoutChangeInfo> layoutChangeInfoList;

		// 텍스트에서 해당 태그를 찾아서 시작, 끝 위치를 반환하고 해당 태그를 삭제한다.
		auto GetTagPos = [](std::wstring& text, const std::wstring& tag) -> std::optional<std::pair<size_t, size_t>>
		{
			const std::wstring startTagName{ TEXT("<") + tag + TEXT(">") };
			const std::wstring endTagName{ TEXT("</") + tag + TEXT(">") };

			auto posStart{ text.find(startTagName) };
			if (posStart == std::wstring::npos)
				return std::nullopt;

			auto posEnd{ text.find(endTagName, posStart + startTagName.length()) };
			if (posEnd == std::wstring::npos)
				posEnd = text.length();

			text = text.erase(posStart, startTagName.length());
			text = text.erase(posEnd - startTagName.length(), endTagName.length());

			return std::make_pair(posStart, posEnd);
		};

		// layoutChangeType을 제외한 다른 타입들의 태그들의 위치와 길이를 조정한다.
		auto UpdateLayoutChangeInfoList = [&](LayoutChangeType layoutChangeType, const std::wstring& tag, size_t startPos, size_t endPos)
		{
			const std::wstring startTagName{ TEXT("<") + tag + TEXT(">") };
			const std::wstring endTagName{ TEXT("</") + tag + TEXT(">") };
			for (int i = 0; i < layoutChangeInfoList.size(); ++i)
			{
				auto& info{ layoutChangeInfoList[i] };
				if (info.changeType == layoutChangeType)
					continue;

				// <big><b></b></big>
				if (info.textRange.startPosition <= startPos &&
					info.textRange.startPosition + info.textRange.length >= static_cast<UINT32>(endPos))
					info.textRange.length -= startTagName.length() + endTagName.length();

				// <big><b></big>
				else if (info.textRange.startPosition <= startPos && startPos <= info.textRange.startPosition + info.textRange.length &&
					info.textRange.startPosition + info.textRange.length <= static_cast<UINT32>(endPos))
					info.textRange.length -= startTagName.length();

				// <big></b></big>
				else if (info.textRange.startPosition <= endPos && endPos <= info.textRange.startPosition + info.textRange.length &&
					info.textRange.startPosition >= startPos)
					info.textRange.length -= endTagName.length();

				if (info.textRange.startPosition >= startPos)
					info.textRange.startPosition -= startTagName.length();
				if (info.textRange.startPosition >= endPos)
					info.textRange.startPosition -= endTagName.length();
			}
		};

		// <big></big>
		while (true)
		{
			const std::wstring tag{ TEXT("big") };
			auto startEndPos = GetTagPos(text, tag);
			if (!startEndPos)
				break;

			const auto& [startPos, endPos] = startEndPos.value();

			LayoutChangeInfo info{ LayoutChangeType::SIZE };
			info.startTagName = std::wstring{ TEXT("<") + tag + TEXT(">") };
			info.endTagName = TEXT("</") + tag + TEXT(">");
			info.fontSize = 24.0f;
			info.textRange = DWRITE_TEXT_RANGE{ static_cast<UINT32>(startPos), static_cast<UINT32>(endPos - (startPos + info.startTagName.length())) };
			layoutChangeInfoList.push_back(std::move(info));

			UpdateLayoutChangeInfoList(LayoutChangeType::SIZE, tag, startPos, endPos);
		}

		// <b></b>
		while (true)
		{
			const std::wstring tag{ TEXT("b") };
			auto startEndPos = GetTagPos(text, tag);
			if (!startEndPos)
				break;

			const auto& [startPos, endPos] = startEndPos.value();

			LayoutChangeInfo info{ LayoutChangeType::WEIGHT };
			info.startTagName = std::wstring{ TEXT("<") + tag + TEXT(">") };
			info.endTagName = TEXT("</") + tag + TEXT(">");
			info.fontWeight = DWRITE_FONT_WEIGHT_BOLD;
			info.textRange = DWRITE_TEXT_RANGE{ static_cast<UINT32>(startPos), static_cast<UINT32>(endPos - (startPos + info.startTagName.length())) };
			layoutChangeInfoList.push_back(std::move(info));

			UpdateLayoutChangeInfoList(LayoutChangeType::WEIGHT, tag, startPos, endPos);
		}

		return layoutChangeInfoList;
	}
}