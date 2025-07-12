module;

// Windows
#include <wrl.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include "External/DirectX/d3dx12.h"

// ImGui
#ifdef _IMGUI
#include "External/ImGui/imgui.h"
#endif

export module Library.Graphics.D3D:RenderTarget;

import std;

using Microsoft::WRL::ComPtr;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics::D3D
{
	class Descriptor;

	class RenderTarget
	{
	public:
		GRAPHICS_API RenderTarget(UINT width, UINT height, const std::array<float, 4>& clearColor = { 0.15625f, 0.171875f, 0.203125f, 1.0f });
		GRAPHICS_API ~RenderTarget();

		ComPtr<ID3D12Resource> GetResource() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetCpuHandle() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilCpuHandle() const;
		const D3D12_VIEWPORT* GetViewport() const;
		const D3D12_RECT* GetScissorRect() const;
		const float* GetClearColor() const;

#ifdef _IMGUI
		ImTextureID GetImGuiTextureID() const;
		ImVec2 GetSize() const;
#endif

	private:
#ifdef _IMGUI
		Descriptor* m_srvDesc;
		ImVec2 m_size;
#endif

		ComPtr<ID3D12Resource> m_renderTarget;
		Descriptor* m_rtvDesc;

		ComPtr<ID3D12Resource> m_depthStencil;
		Descriptor* m_dsvDesc;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		std::array<float, 4> m_clearColor;
	};
}
