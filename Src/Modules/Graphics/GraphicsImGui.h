#pragma once

#ifdef _IMGUI
namespace Graphics::D3D
{
	class RenderTarget;
}

namespace Graphics::ImGui
{
	class Texture;

	DLL_API void Begin();
	DLL_API void End();

	DLL_API ImGuiContext* GetContext();
	DLL_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DLL_API std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path);
	DLL_API void Image(const std::shared_ptr<Texture>& texture, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, const ImVec4& tint_col = ImVec4{ 1, 1, 1, 1 }, const ImVec4& border_col = ImVec4{ 0, 0, 0, 0 });
	DLL_API void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size = ImVec2{ 0, 0 }, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, const ImVec4& tint_col = ImVec4{ 1, 1, 1, 1 }, const ImVec4& border_col = ImVec4{ 0, 0, 0, 0 });
}
#endif
