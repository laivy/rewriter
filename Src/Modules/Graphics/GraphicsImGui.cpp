#include "Stdafx.h"
#ifdef _IMGUI
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"
#include "Graphics3DUtil.h"
#include "GraphicsImGui.h"
#include "RenderTarget.h"
#include "Shared/Util.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Graphics::ImGui
{
	class Texture
	{
	public:
		Texture(const ComPtr<ID3D12Resource>& resource) :
			m_resource{ resource },
			m_descriptor{ nullptr },
			m_size{}
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = resource->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;

			auto dm{ D3D::DescriptorManager::GetInstance() };
			m_descriptor = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			m_descriptor->CreateShaderResourceView(m_resource, &srvDesc);

			auto desc{ m_resource->GetDesc() };
			m_size.x = static_cast<float>(desc.Width);
			m_size.y = static_cast<float>(desc.Height);
		}

		~Texture()
		{
			auto dm{ D3D::DescriptorManager::GetInstance() };
			dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_descriptor);
		}

		ImTextureID GetImGuiTextureID() const
		{
			if (m_descriptor)
				return reinterpret_cast<ImTextureID>(m_descriptor->GetGpuHandle().ptr);
			return 0;
		}

		ImVec2 GetSize() const
		{
			return m_size;
		}

	private:
		ComPtr<ID3D12Resource> m_resource;
		D3D::Descriptor* m_descriptor;
		ImVec2 m_size;
	};

	DLL_API void Begin()
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

	DLL_API std::shared_ptr<Texture> LoadTexture(const std::filesystem::path& path)
	{
		ComPtr<ID3D12Resource> resource;
		std::unique_ptr<uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource{};
		if (FAILED(DirectX::LoadWICTextureFromFileEx(g_d3dDevice.Get(), path.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32, &resource, decodedData, subresource)))
			return nullptr;
		if (!D3D::CopyResource(resource, subresource))
			return nullptr;
		return std::make_shared<Texture>(resource);
	}

	void Image(const std::shared_ptr<Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		::ImGui::Image(texture->GetImGuiTextureID(), (size.x == 0.0f && size.y == 0.0f) ? texture->GetSize() : size, uv0, uv1, tint_col, border_col);
	}

	void Image(const std::shared_ptr<D3D::RenderTarget>& renderTarget, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
	{
		::ImGui::Image(renderTarget->GetImGuiTextureID(), size, uv0, uv1, tint_col, border_col);
	}
}
#endif
