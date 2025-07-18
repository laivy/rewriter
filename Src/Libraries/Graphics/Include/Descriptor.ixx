module;

// Windows
#include <wrl.h>

// DirectX
#include "External/DirectX/d3dx12.h"

export module rewriter.library.graphics.direct3d:descriptor;

using Microsoft::WRL::ComPtr;

export namespace Graphics::D3D
{
	class Descriptor
	{
	public:
		Descriptor();
		Descriptor(CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle, CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle);
		~Descriptor() = default;

		Descriptor(const Descriptor&) = delete;
		Descriptor& operator=(const Descriptor&) = delete;

		Descriptor(Descriptor&& rhs) noexcept;
		Descriptor& operator=(Descriptor&& rhs) noexcept;

		void CreateShaderResourceView(const ComPtr<ID3D12Resource>& resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc) const;
		void CreateRenderTargetView(const ComPtr<ID3D12Resource>& resource, const D3D12_RENDER_TARGET_VIEW_DESC* desc) const;
		void CreateDepthStencilView(const ComPtr<ID3D12Resource>& resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* desc) const;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const;

	private:
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cpuHandle;
		CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle;
	};
}
