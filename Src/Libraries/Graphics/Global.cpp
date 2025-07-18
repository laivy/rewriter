module;

// Windows
#include <wrl.h>

// DirectX
#include <dxgi1_6.h>

#ifdef _DIRECT2D
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <dwrite_3.h>
#endif

module rewriter.library.graphics.global;

import std;
import rewriter.library.graphics.direct3d;

using Microsoft::WRL::ComPtr;

namespace Graphics
{
	// Windows
	void* g_hWnd;

	// D3D12
	ComPtr<IDXGIFactory4> g_dxgiFactory;
	ComPtr<ID3D12Device> g_d3dDevice;
	ComPtr<ID3D12CommandQueue> g_commandQueue;
	ComPtr<ID3D12CommandAllocator> g_uploadCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> g_uploadCommandList;
	std::unique_ptr<D3D::SwapChain> g_swapChain;
	ComPtr<ID3D12GraphicsCommandList> g_commandList;
	ComPtr<ID3D12RootSignature> g_rootSignature;

#ifdef _DIRECT2D
	// D3D11on12
	ComPtr<ID3D11On12Device> g_d3d11On12Device;
	ComPtr<ID3D11DeviceContext> g_d3d11DeviceContext;

	// D2D
	ComPtr<ID2D1Factory3> g_d2dFactory;
	ComPtr<ID2D1Device2> g_d2dDevice;
	ComPtr<ID2D1DeviceContext2> g_d2dContext;
	std::vector<ID2D1RenderTarget*> g_d2dCurrentRenderTargets;
	ComPtr<IDWriteFactory5> g_dwriteFactory;
#endif

	// 로직에 사용되는 변수들
	std::vector<ComPtr<ID3D12Resource>> g_uploadBuffers;
	std::shared_ptr<D3D::Camera> g_camera;
}
