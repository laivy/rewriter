module;

#ifdef _IMGUI
#include "External/Imgui/imgui.h"
#endif

export module rewriter.common.util;

import std;

export namespace Util
{
	std::string wstou8s(std::wstring_view wstr);
	std::string wstombs(std::wstring_view wstr);
	std::string u8stou8s(std::u8string_view u8str);
	std::string u8stombs(std::u8string_view u8str);
	std::wstring u8stows(std::string_view u8str);
}

#ifdef _IMGUI
export namespace ImGui
{
	// C++ 표준 호환용 함수
	bool Button(const std::string& label, const ImVec2& size = ImVec2{ 0, 0 });
	bool Button(const std::wstring& label, const ImVec2& size = ImVec2{ 0, 0 });
	bool Button(const std::u8string& label, const ImVec2& size = ImVec2{ 0, 0 });
	bool Selectable(const std::string& label, bool selected = false, ImGuiSelectableFlags flags = ImGuiSelectableFlags_None, const ImVec2& size = ImVec2{ 0, 0 });
	bool Selectable(const std::wstring& label, bool selected = false, ImGuiSelectableFlags flags = ImGuiSelectableFlags_None, const ImVec2& size = ImVec2{ 0, 0 });
	bool Selectable(const std::u8string& label, bool selected = false, ImGuiSelectableFlags flags = ImGuiSelectableFlags_None, const ImVec2& size = ImVec2{ 0, 0 });
}
#endif
