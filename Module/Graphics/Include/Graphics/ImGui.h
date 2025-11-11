#pragma once
#ifdef _IMGUI
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <Resource/Resource.h>
#include "Export.h"

namespace Graphics::ImGui
{
	GRAPHICS_API ImGuiContext* GetContext();
	GRAPHICS_API LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	GRAPHICS_API void Begin();
	GRAPHICS_API void End();

	GRAPHICS_API ImTextureID GetImage(Resource::ID id);
	GRAPHICS_API ImTextureID GetImage(const std::filesystem::path& path);
	GRAPHICS_API ImVec2 GetImageSize(ImTextureID id);
}
#endif
