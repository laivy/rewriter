#pragma once
#include "Export.h"

#ifdef _IMGUI
namespace Graphics::D3D
{
	class RenderTarget;
}

namespace Graphics::ImGui
{
	struct Texture;

	GRAPHICS_API void Begin();
	GRAPHICS_API void End();

	GRAPHICS_API ImGuiContext* GetContext();
	GRAPHICS_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	GRAPHICS_API std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path);
	GRAPHICS_API ImTextureRef GetTextureRef(const std::shared_ptr<Texture>& texture);
	GRAPHICS_API ImVec2 GetTextureSize(const std::shared_ptr<Texture>& texture);
	GRAPHICS_API void Image(const std::shared_ptr<Texture>& texture, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 });
	GRAPHICS_API void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 });
}
#endif
