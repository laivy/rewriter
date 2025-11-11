#include "Pch.h"
#ifdef _IMGUI
#include "Context.h"
#include "../Include/Graphics/ImGui.h"
#include "ImGui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Graphics::ImGui
{
	ImGuiContext* GetContext()
	{
		return ::ImGui::GetCurrentContext();
	}

	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return ::ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	}

	void Begin()
	{
		::ImGui_ImplDX12_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();
	}

	void End()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return;

		::ImGui::Render();
		::ImGui_ImplDX12_RenderDrawData(::ImGui::GetDrawData(), ctx->commandList.Get());
		if (::ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
	}

	ImTextureID GetImage(Resource::ID id)
	{
		auto path{ Resource::GetPath(id) };
		if (!path)
			return ImTextureID{};
		return GetImage(*path);
	}

	ImTextureID GetImage(const std::filesystem::path& path)
	{
		auto ctx{ Graphics::ImGui::Context::GetInstance() };
		if (!ctx)
			return ImTextureID{};

		if (!ctx->textures.contains(path))
			return ImTextureID{};

		return ctx->textures.at(path).id;
	}

	ImVec2 GetImageSize(ImTextureID id)
	{
		auto ctx{ Graphics::ImGui::Context::GetInstance() };
		if (!ctx)
			return ImVec2{ 0.0f, 0.0f };

		for (const auto& texture : ctx->textures | std::views::values)
		{
			if (texture.id == id)
				return ImVec2{ static_cast<float>(texture.width), static_cast<float>(texture.height) };
		}
		return ImVec2{ 0.0f, 0.0f };
	}
}
#endif
