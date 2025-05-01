#pragma once

namespace Graphics::D3D
{
	class Descriptor;

	class RenderTarget
	{
	public:
		DLL_API RenderTarget(UINT width, UINT height);
		DLL_API ~RenderTarget();

		ComPtr<ID3D12Resource> GetResource() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCpuHandle() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilCpuHandle() const;

#ifdef _IMGUI
		ImTextureID GetImGuiTextureID() const;
#endif

	private:
#ifdef _IMGUI
		Descriptor* m_srvDesc;
#endif

		ComPtr<ID3D12Resource> m_renderTarget;
		Descriptor* m_rtvDesc;

		ComPtr<ID3D12Resource> m_depthStencil;
		Descriptor* m_dsvDesc;
	};
}
