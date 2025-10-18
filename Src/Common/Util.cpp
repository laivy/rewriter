#include "Pch.h"
#include "Util.h"

namespace Util
{
	std::string ToU8String(std::wstring_view wstr)
	{
		const int length{ ::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		if (!::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL))
			assert(false);
		return str;
	}
}

#ifdef _IMGUI
namespace ImGui
{
	bool Button(const std::string& label, const ImVec2& size)
	{
		return Button(label.c_str(), size);
	}

	bool Button(const std::wstring& label, const ImVec2& size)
	{
		return Button(Util::ToU8String(label).c_str(), size);
	}

	bool Button(const std::u8string& label, const ImVec2& size)
	{
		return Button(reinterpret_cast<const char*>(label.c_str()), size);
	}

	bool Selectable(const std::string& label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
	{
		return Selectable(label.c_str(), selected, flags, size);
	}

	bool Selectable(const std::wstring& label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
	{
		return Selectable(Util::ToU8String(label).c_str(), selected, flags, size);
	}

	bool Selectable(const std::u8string& label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
	{
		return Selectable(reinterpret_cast<const char*>(label.c_str()), selected, flags, size);
	}
}
#endif
