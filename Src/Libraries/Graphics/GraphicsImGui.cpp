module;

// Windows
#include <wrl.h>

// DirectX
#include <d3d12.h>
#include "External/DirectX/WICTextureLoader12.h"

// ImGui
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_internal.h"
#include "External/ImGui/imgui_impl_dx12.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/Imgui/imgui_stdlib.h"

module rewriter.library.graphics.direct3d:imgui;

import std;
import :descriptor;
import :descriptor_manager;
import :util;
import rewriter.library.graphics.global;

using Microsoft::WRL::ComPtr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
	struct FileDialog
	{
		bool isOpened{ false };
		std::string_view name;

		std::filesystem::path path; // 현재 탐색중인 폴더 경로
		std::string filename; // 파일 이름
	};

	std::vector<FileDialog> g_fileDialogs;
}

namespace Graphics::ImGui
{
	struct Texture
	{
		ComPtr<ID3D12Resource> resource;
		D3D::Descriptor* descriptor;
		ImVec2 size;
	};

	void Begin()
	{
		if (auto dm{ D3D::DescriptorManager::GetInstance() })
			dm->SetDescriptorHeaps();

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

	void End()
	{
		::ImGui::Render();
		::ImGui_ImplDX12_RenderDrawData(::ImGui::GetDrawData(), g_commandList.Get());
		if (::ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
	}

	ImGuiContext* GetContext()
	{
		return ::ImGui::GetCurrentContext();
	}

	LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return ::ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}

	std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path)
	{
		ComPtr<ID3D12Resource> resource;
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource{};
		if (FAILED(DirectX::LoadWICTextureFromFileEx(g_d3dDevice.Get(), path.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32, &resource, decodedData, subresource)))
			return nullptr;
		if (!D3D::CopyResource(resource, subresource))
			return nullptr;

		auto texture{ std::make_shared<Texture>(resource) };

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = resource->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;

		auto dm{ D3D::DescriptorManager::GetInstance() };
		texture->descriptor = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		texture->descriptor->CreateShaderResourceView(texture->resource, &srvDesc);

		auto desc{ texture->resource->GetDesc() };
		texture->size.x = static_cast<float>(desc.Width);
		texture->size.y = static_cast<float>(desc.Height);

		return texture;
	}

	void Image(const std::shared_ptr<Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		::ImGui::Image(texture->descriptor->GetGpuHandle().ptr, (size.x == 0.0f && size.y == 0.0f) ? texture->size : size, uv0, uv1, tint_col, border_col);
	}

	void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		::ImGui::Image(renderTarget->GetImGuiTextureID(), size, uv0, uv1, tint_col, border_col);
	}

	void OpenFileDialog(std::string_view str_id)
	{
		FileDialog dialog{};
		dialog.name = str_id;
		dialog.path = std::filesystem::current_path();
		g_fileDialogs.push_back(dialog);
	}

	bool BeginFileDialog(std::string_view name, bool* p_open, ImGuiWindowFlags flags)
	{
		auto it{ std::ranges::find_if(g_fileDialogs, [name](const auto& dialog) { return dialog.name == name; }) };
		if (it == g_fileDialogs.end())
			return false;

		auto& dialog{ *it };
		if (!dialog.isOpened)
		{
			dialog.isOpened = true;
			::ImGui::OpenPopup(dialog.name.data());
		}

		::ImGui::SetNextWindowSize(ImVec2{ 700.0f, 400.0f }, ImGuiCond_FirstUseEver);
		if (!::ImGui::BeginPopupModal(dialog.name.data(), p_open, flags | ImGuiWindowFlags_NoSavedSettings))
		{
			g_fileDialogs.erase(it);
			return false;
		}

		// 주소창
		std::string temp{ dialog.path.string() };
		::ImGui::SetNextItemWidth(-1.0f);
		if (::ImGui::InputText("##Path", &temp, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			if (std::filesystem::exists(temp))
				dialog.path = temp;
			else
				dialog.path = std::filesystem::current_path();
		}
		::ImGui::Separator();

		// 폴더, 파일 목록
		constexpr auto BottomAreaHeight{ 60.0f };
		if (::ImGui::BeginChild("Folders", ImVec2{ 100.0f, -BottomAreaHeight }, ImGuiChildFlags_ResizeX))
		{
			if (::ImGui::Selectable("..", false, ImGuiSelectableFlags_DontClosePopups))
			{
				if (dialog.path.has_parent_path())
					dialog.path = dialog.path.parent_path();
			}

			for (const auto& entry : std::filesystem::directory_iterator{ dialog.path })
			{
				if (!entry.is_directory())
					continue;

				if (::ImGui::Selectable(reinterpret_cast<const char*>(entry.path().filename().u8string().c_str()), false, ImGuiSelectableFlags_DontClosePopups))
					dialog.path = entry.path();
			}
		}
		::ImGui::EndChild();
		::ImGui::SameLine();
		::ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		::ImGui::SameLine();
		if (::ImGui::BeginChild("Files", ImVec2{ 0.0f, -BottomAreaHeight }))
		{
			if (::ImGui::BeginTable("Info", 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
			{
				const float CharacterWidth = ::ImGui::CalcTextSize("A").x;

				::ImGui::TableSetupColumn("File");
				::ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 8.0f);
				::ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 5.0f);
				::ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, CharacterWidth * 15.0f);
				::ImGui::TableHeadersRow();
				for (const auto& entry : std::filesystem::directory_iterator{ dialog.path })
				{
					if (!entry.is_regular_file())
						continue;

					const auto& path{ entry.path() };

					::ImGui::TableNextRow();
					::ImGui::TableNextColumn();
					if (::ImGui::Selectable(path.filename().string().c_str(), path == dialog.path / dialog.filename, ImGuiSelectableFlags_SpanAllColumns))
						dialog.filename = path.filename().string();

					::ImGui::TableNextColumn();
					::ImGui::Text("%zuKB", std::filesystem::file_size(path) / 1'000);

					::ImGui::TableNextColumn();
					::ImGui::TextUnformatted(path.extension().string().c_str());

					::ImGui::TableNextColumn();
					::ImGui::TextUnformatted(std::format("{0:%F %T}", std::chrono::floor<std::chrono::seconds>(entry.last_write_time())).c_str());
				}
				::ImGui::EndTable();
			}
		}
		::ImGui::EndChild();

		// 하단 영역
		::ImGui::Separator();
		::ImGui::AlignTextToFramePadding();
		::ImGui::TextUnformatted("File Name:");
		::ImGui::SameLine();

		::ImGui::SetNextItemWidth(-1.0f);
		::ImGui::InputText("##File Name", &dialog.filename);
		if (::ImGui::Button("Open"))
		{
		}
		::ImGui::SameLine();
		if (::ImGui::Button("Close"))
			::ImGui::CloseCurrentPopup();
		return true;
	}
}
