#include "Stdafx.h"
#include "Util.h"

namespace Util
{
	std::string wstou8s(std::wstring_view wstr)
	{
		int length{ ::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		::WideCharToMultiByte(CP_UTF8, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL);
		return str;
	}

	std::string wstombs(std::wstring_view wstr)
	{
		int length{ ::WideCharToMultiByte(CP_ACP, NULL, wstr.data(), static_cast<int>(wstr.size()), NULL, NULL, NULL, NULL) };
		std::string str(length, '\0');
		::WideCharToMultiByte(CP_ACP, NULL, wstr.data(), static_cast<int>(wstr.size()), str.data(), length, NULL, NULL);
		return str;
	}

	// std::u8string -> std::string
	std::string u8stou8s(std::u8string_view u8str)
	{
		return std::string{ u8str.begin(), u8str.end() };
	}

	std::string u8stombs(std::u8string_view u8str)
	{
		int length{ ::MultiByteToWideChar(CP_UTF8, NULL, reinterpret_cast<const char*>(u8str.data()), static_cast<int>(u8str.size()), NULL, NULL) };
		std::wstring wstr(length, '\0');
		::MultiByteToWideChar(CP_UTF8, NULL, reinterpret_cast<const char*>(u8str.data()), static_cast<int>(u8str.size()), wstr.data(), length);
		return wstombs(wstr);
	}

	std::wstring u8stows(std::string_view u8str)
	{
		int length{ ::MultiByteToWideChar(CP_UTF8, NULL, u8str.data(), static_cast<int>(u8str.size()), NULL, NULL) };
		std::wstring wstr(length, '\0');
		::MultiByteToWideChar(CP_UTF8, NULL, u8str.data(), static_cast<int>(u8str.size()), wstr.data(), length);
		return wstr;
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
		return Button(Util::wstou8s(label).c_str(), size);
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
		return Selectable(Util::wstou8s(label).c_str(), selected, flags, size);
	}

	bool Selectable(const std::u8string& label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
	{
		return Selectable(reinterpret_cast<const char*>(label.c_str()), selected, flags, size);
	}
}
#endif
