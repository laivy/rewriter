module;

#include "External/Imgui/imgui.h"

export module rewriter.library.graphics.direct3d:filedialog;

import std;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics::ImGui::FileDialog
{
	GRAPHICS_API void Open(std::string_view str_id);
	GRAPHICS_API bool Begin(std::string_view name, bool* p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
	GRAPHICS_API std::filesystem::path GetPath();
}
