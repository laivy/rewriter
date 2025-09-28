#pragma once

namespace Util
{
	std::string ToU8String(std::wstring_view wstr);
}

#ifdef _IMGUI
namespace ImGui
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
