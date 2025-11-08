#include "Pch.h"
#include "Descriptor.h"
#include "Global.h"

namespace Graphics::D3D
{
	Descriptor::Descriptor() :
		m_cpuHandle{ D3D12_DEFAULT },
		m_gpuHandle{ D3D12_DEFAULT }
	{
	}

	Descriptor::Descriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle) :
		m_cpuHandle{ cpuHandle },
		m_gpuHandle{ gpuHandle }
	{
	}

	Descriptor::Descriptor(Descriptor&& rhs) noexcept
	{
		m_cpuHandle = rhs.m_cpuHandle;
		m_gpuHandle = rhs.m_gpuHandle;
		rhs.m_cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
		rhs.m_gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
	}

	Descriptor& Descriptor::operator=(Descriptor&& rhs) noexcept
	{
		m_cpuHandle = rhs.m_cpuHandle;
		m_gpuHandle = rhs.m_gpuHandle;
		rhs.m_cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
		rhs.m_gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
		return *this;
	}

	void Descriptor::CreateShaderResourceView(const ComPtr<ID3D12Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc) const
	{
		g_d3dDevice->CreateShaderResourceView(resource.Get(), desc, m_cpuHandle);
	}

	void Descriptor::CreateRenderTargetView(const ComPtr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc) const
	{
		g_d3dDevice->CreateRenderTargetView(resource.Get(), desc, m_cpuHandle);
	}

	void Descriptor::CreateDepthStencilView(const ComPtr<ID3D12Resource>& resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc) const
	{
		g_d3dDevice->CreateDepthStencilView(resource.Get(), desc, m_cpuHandle);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE Descriptor::GetCpuHandle() const
	{
		return m_cpuHandle;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE Descriptor::GetGpuHandle() const
	{
		return m_gpuHandle;
	}
}
