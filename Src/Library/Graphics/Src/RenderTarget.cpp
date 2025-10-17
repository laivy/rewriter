#include "Pch.h"
#include "Global.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "RenderTarget.h"

namespace Graphics::D3D
{
	RenderTarget::RenderTarget(UINT width, UINT height, const std::array<float, 4>& clearColor) :
#ifdef _IMGUI
		m_srvDesc{ nullptr },
#endif
		m_rtvDesc{ nullptr },
		m_dsvDesc{ nullptr },
		m_viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f },
		m_scissorRect{ 0, 0, static_cast<long>(width), static_cast<long>(height) },
		m_clearColor{ clearColor }
	{
		// 렌더타겟
		{
			D3D12_RESOURCE_DESC desc{};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			CD3DX12_HEAP_PROPERTIES heapProperties{ D3D12_HEAP_TYPE_DEFAULT };

			constexpr float color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
			CD3DX12_CLEAR_VALUE clearValue{ DXGI_FORMAT_R8G8B8A8_UNORM, color };

			g_d3dDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COMMON,
				&clearValue,
				IID_PPV_ARGS(&m_renderTarget)
			);
		}

		// 깊이스텐실
		{
			D3D12_RESOURCE_DESC desc{};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Width = width;
			desc.Height = height;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			CD3DX12_HEAP_PROPERTIES heapProperties{ D3D12_HEAP_TYPE_DEFAULT };
			CD3DX12_CLEAR_VALUE clearValue{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 };

			g_d3dDevice->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&m_depthStencil)
			);
		}

		// 뷰 생성
		if (auto dm{ DescriptorManager::GetInstance() })
		{
#ifdef _IMGUI
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Texture2D.MipLevels = 1;
			m_srvDesc = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			m_srvDesc->CreateShaderResourceView(m_renderTarget, &srvDesc);

			m_size.x = static_cast<float>(width);
			m_size.y = static_cast<float>(height);
#endif

			m_rtvDesc = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_rtvDesc->CreateRenderTargetView(m_renderTarget, nullptr);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			m_dsvDesc = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_dsvDesc->CreateDepthStencilView(m_depthStencil, &dsvDesc);
		}
	}

	RenderTarget::~RenderTarget()
	{
		if (auto dm{ DescriptorManager::GetInstance() })
		{
#ifdef _IMGUI
			dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_srvDesc);
#endif
			dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_rtvDesc);
			dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_dsvDesc);
		}
	}

	ComPtr<ID3D12Resource> RenderTarget::GetResource() const
	{
		return m_renderTarget;
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetRenderTargetCpuHandle() const
	{
		if (m_rtvDesc)
			return m_rtvDesc->GetCpuHandle();
		return CD3DX12_CPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE RenderTarget::GetDepthStencilCpuHandle() const
	{
		if (m_dsvDesc)
			return m_dsvDesc->GetCpuHandle();
		return CD3DX12_CPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
	}

	const D3D12_VIEWPORT* RenderTarget::GetViewport() const
	{
		return &m_viewport;
	}

	const D3D12_RECT* RenderTarget::GetScissorRect() const
	{
		return &m_scissorRect;
	}

	const float* RenderTarget::GetClearColor() const
	{
		return m_clearColor.data();
	}

#ifdef _IMGUI
	ImTextureID RenderTarget::GetImGuiTextureID() const
	{
		if (m_srvDesc)
			return m_srvDesc->GetGpuHandle().ptr;
		return 0;
	}

	ImVec2 RenderTarget::GetSize() const
	{
		return m_size;
	}
#endif
}
