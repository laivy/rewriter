#pragma once
#ifdef _IMGUI
// C++ 표준 라이브러리
#include <utility>

// Windows
#include <Windows.h>

// ImGui
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

// 프로젝트 모듈
#include <Resource/Resource.h>

// 프로젝트
#include "Export.h"
#include "ImGuiFileDialog.h"

namespace Graphics::ImGui
{
	GRAPHICS_API ImGuiContext* GetContext();
	GRAPHICS_API LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	GRAPHICS_API void Begin();
	GRAPHICS_API void End();

#ifdef _TOOL
	GRAPHICS_API std::pair<ImTextureID, ImVec2> Image(Resource::ID id);
	GRAPHICS_API std::pair<ImTextureID, ImVec2> Image(const std::wstring& path);
	GRAPHICS_API void DeleteImage(Resource::ID id);
#endif
}
#endif
