module;

#include <wrl.h>
#include "External/ImGui/imgui.h"

export module Library.Graphics.D3D:ImGui;

import std;
import :RenderTarget;

using Microsoft::WRL::ComPtr;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics::ImGui
{
	struct Texture;

	GRAPHICS_API void Begin();
	GRAPHICS_API void End();

	GRAPHICS_API ImGuiContext* GetContext();
	GRAPHICS_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	GRAPHICS_API std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path);
	GRAPHICS_API void Image(const std::shared_ptr<Texture>& texture, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, const ImVec4& tint_col = ImVec4{ 1, 1, 1, 1 }, const ImVec4& border_col = ImVec4{ 0, 0, 0, 0 });
	GRAPHICS_API void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, const ImVec4& tint_col = ImVec4{ 1, 1, 1, 1 }, const ImVec4& border_col = ImVec4{ 0, 0, 0, 0 });
}
