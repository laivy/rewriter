#include "Stdafx.h"
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

	DLL_API bool Button(std::wstring_view label, const ImVec2& size)
	{
		return ::ImGui::Button(Util::wstou8s(label).c_str(), size);
	}
}
