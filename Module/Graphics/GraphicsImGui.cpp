#include "Stdafx.h"
#ifdef _IMGUI
#include "Global.h"
#include "GraphicsImGui.h"
#include "Common/Util.h"

namespace Graphics::ImGui
{
	DLL_API void Begin()
	{
		commandList->SetDescriptorHeaps(1, imGuiSrvDescHeap.GetAddressOf());
		::ImGui_ImplDX12_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();

#ifdef _SERVER
		auto viewport{ ::ImGui::GetMainViewport() };
		::ImGui::SetNextWindowPos(viewport->WorkPos);
		::ImGui::SetNextWindowSize(viewport->WorkSize);
		::ImGui::SetNextWindowViewport(viewport->ID);
		::ImGui::Begin("DOCKSPACE", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking
		);
		::ImGui::DockSpace(::ImGui::GetID("DOCKSPACE"), ImVec2{}, ImGuiDockNodeFlags_PassthruCentralNode);
		::ImGui::End();
#endif
	}

	DLL_API void End()
	{
		::ImGui::Render();
		::ImGui_ImplDX12_RenderDrawData(::ImGui::GetDrawData(), commandList.Get());
		if (::ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
	}

	DLL_API ImGuiContext* GetContext()
	{
		return ::ImGui::GetCurrentContext();
	}

	DLL_API LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ::ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	DLL_API bool Button(std::wstring_view label, const ImVec2& size)
	{
		return ::ImGui::Button(Util::wstou8s(label).c_str(), size);
	}
}
#endif
