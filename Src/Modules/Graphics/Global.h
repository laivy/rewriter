#pragma once

namespace Graphics
{
	namespace D3D
	{
		class Camera;
	}

	constexpr auto FRAME_COUNT{ 3U };

	// Windows
	extern HWND g_hWnd;

	// D3D12
	extern ComPtr<IDXGIFactory4> g_dxgiFactory;
	extern ComPtr<IDXGISwapChain3>	g_swapChain;
	extern ComPtr<ID3D12Device> g_d3dDevice;
	extern ComPtr<ID3D12CommandQueue> g_commandQueue;
	extern std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> g_commandAllocators;
	extern ComPtr<ID3D12GraphicsCommandList> g_commandList;
	extern ComPtr<ID3D12DescriptorHeap> g_rtvDescHeap;
	extern ComPtr<ID3D12DescriptorHeap> g_dsvDescHeap;
	extern ComPtr<ID3D12DescriptorHeap> g_srvDescHeap;
	extern std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> g_renderTargets;
	extern ComPtr<ID3D12Resource> g_depthStencil;
	extern ComPtr<ID3D12RootSignature> g_rootSignature;
	extern ComPtr<ID3D12Fence> g_fence;
	extern D3D12_VIEWPORT g_viewport;
	extern D3D12_RECT g_scissorRect;
	extern HANDLE g_fenceEvent;
	extern std::array<UINT64, FRAME_COUNT> g_fenceValues;
	extern UINT g_frameIndex;
	extern UINT g_rtvDescriptorSize;
	extern UINT g_cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	extern ComPtr<ID3D11On12Device> g_d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> g_d3d11DeviceContext;
	extern std::array<ComPtr<ID3D11Resource>, FRAME_COUNT> g_wrappedBackBuffers;

	// D2D
	extern ComPtr<ID2D1DeviceContext2> g_d2dContext;
	extern ComPtr<ID2D1Factory3> g_d2dFactory;
	extern ComPtr<ID2D1Device2> g_d2dDevice;
	extern std::array<ComPtr<ID2D1Bitmap1>, FRAME_COUNT> g_d2dRenderTargets;
	extern std::vector<ID2D1RenderTarget*> g_d2dCurrentRenderTargets;
	extern ComPtr<IDWriteFactory5> g_dwriteFactory;

#ifdef _IMGUI
	// IMGUI
	extern ComPtr<ID3D12DescriptorHeap> g_imGuiSrvDescHeap;
#endif

	// 로직에 사용되는 변수들
	extern std::vector<ComPtr<ID3D12Resource>> g_uploadBuffers;
	extern std::shared_ptr<D3D::Camera> g_camera;
	extern Int2 g_renderTargetSize;
}
