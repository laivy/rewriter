#pragma once

#ifdef _IMGUI
namespace Graphics::ImGui
{
	DLL_API void Begin();
	DLL_API void End();

	DLL_API ImGuiContext* GetContext();
	DLL_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
#endif
