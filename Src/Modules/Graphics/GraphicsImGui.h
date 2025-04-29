#pragma once

#ifdef _IMGUI
namespace Graphics::D3D
{
	class RenderTarget;
}

namespace Graphics::ImGui
{
	DLL_API void Begin();
	DLL_API void End();

	DLL_API ImGuiContext* GetContext();
	DLL_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	DLL_API void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size, const ImVec2& uv0 = ImVec2{ 0, 0 }, const ImVec2& uv1 = ImVec2{ 1, 1 }, const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4{ 0, 0, 0, 0 });
}
#endif
