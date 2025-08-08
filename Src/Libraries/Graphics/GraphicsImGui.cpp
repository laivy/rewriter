module;

// Windows
#include <wrl.h>

// DirectX
#include <d3d12.h>
#include "External/DirectX/WICTextureLoader12.h"

// ImGui
#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_dx12.h"
#include "External/ImGui/imgui_impl_win32.h"

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
}
