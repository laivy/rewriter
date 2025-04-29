#include "Stdafx.h"
#ifdef _IMGUI
#include "DescriptorManager.h"
#include "Global.h"
#include "GraphicsImGui.h"
#include "RenderTarget.h"
#include "Shared/Util.h"

namespace Graphics::ImGui
{
	DLL_API void Begin()
	{
		auto dm{ D3D::DescriptorManager::GetInstance() };
		if (!dm)
		{
			assert(false);
			return;
		}
		g_commandList->SetDescriptorHeaps(1, dm->GetSrvHeap().GetAddressOf());

		::ImGui_ImplDX12_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();

#if defined _SERVER || defined _TOOL
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
		::ImGui_ImplDX12_RenderDrawData(::ImGui::GetDrawData(), g_commandList.Get());
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

	void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		::ImGui::Image(renderTarget->GetImGuiTextureID(), size, uv0, uv1, tint_col, border_col);
	}
}
#endif
