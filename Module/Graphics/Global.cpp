#include "Stdafx.h"
#include "Global.h"

namespace Graphics
{
	// Windows
	HWND g_hWnd;

	// D3D12
	ComPtr<IDXGIFactory4> g_dxgiFactory;
	ComPtr<IDXGISwapChain3>	g_swapChain;
	ComPtr<ID3D12Device> g_d3dDevice;
	ComPtr<ID3D12CommandQueue> g_commandQueue;
	std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> g_commandAllocators;
	ComPtr<ID3D12GraphicsCommandList> g_commandList;
	ComPtr<ID3D12DescriptorHeap> g_rtvDescHeap;
	ComPtr<ID3D12DescriptorHeap> g_dsvDescHeap;
	ComPtr<ID3D12DescriptorHeap> g_srvDescHeap;
	std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> g_renderTargets;
	ComPtr<ID3D12Resource> g_depthStencil;
	ComPtr<ID3D12RootSignature> g_rootSignature;
	ComPtr<ID3D12Fence> g_fence;
	D3D12_VIEWPORT g_viewport;
	D3D12_RECT g_scissorRect;
	HANDLE g_fenceEvent;
	std::array<UINT64, FRAME_COUNT> g_fenceValues;
	UINT g_frameIndex;
	UINT g_rtvDescriptorSize;
	UINT g_cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	ComPtr<ID3D11On12Device> g_d3d11On12Device;
	ComPtr<ID3D11DeviceContext> g_d3d11DeviceContext;
	std::array<ComPtr<ID3D11Resource>, FRAME_COUNT> g_wrappedBackBuffers;

	// D2D
	ComPtr<ID2D1Factory3> g_d2dFactory;
	ComPtr<ID2D1Device2> g_d2dDevice;
	ComPtr<ID2D1DeviceContext2> g_d2dContext;
	std::array<ComPtr<ID2D1Bitmap1>, FRAME_COUNT> g_d2dRenderTargets;
	std::vector<ID2D1RenderTarget*> g_d2dCurrentRenderTargets;

	// DWRITE
	ComPtr<IDWriteFactory5> g_dwriteFactory;

#ifdef _IMGUI
	// IMGUI
	ComPtr<ID3D12DescriptorHeap> g_imGuiSrvDescHeap;
#endif
}
