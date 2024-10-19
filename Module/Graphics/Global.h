#pragma once

namespace Graphics
{
	constexpr auto FRAME_COUNT{ 3u };

	// Windows
	extern HWND hWnd;

	// D3D12
	extern ComPtr<IDXGIFactory4> dxgiFactory;
	extern ComPtr<IDXGISwapChain3>	swapChain;
	extern ComPtr<ID3D12Device> d3dDevice;
	extern ComPtr<ID3D12CommandQueue> commandQueue;
	extern std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> commandAllocators;
	extern ComPtr<ID3D12GraphicsCommandList> commandList;
	extern ComPtr<ID3D12DescriptorHeap> rtvDescHeap;
	extern ComPtr<ID3D12DescriptorHeap> dsvDescHeap;
	extern ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	extern std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> renderTargets;
	extern ComPtr<ID3D12Resource> depthStencil;
	extern ComPtr<ID3D12RootSignature> rootSignature;
	extern ComPtr<ID3D12Fence> fence;
	extern D3D12_VIEWPORT viewport;
	extern D3D12_RECT scissorRect;
	extern HANDLE fenceEvent;
	extern std::array<UINT64, FRAME_COUNT> fenceValues;
	extern UINT frameIndex;
	extern UINT rtvDescriptorSize;
	extern UINT cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	extern ComPtr<ID3D11On12Device> d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	extern std::array<ComPtr<ID3D11Resource>, FRAME_COUNT> wrappedBackBuffers;

	// D2D
	extern ComPtr<ID2D1DeviceContext2> d2dContext;
	extern ComPtr<ID2D1Factory3> d2dFactory;
	extern ComPtr<ID2D1Device2> d2dDevice;
	extern std::array<ComPtr<ID2D1Bitmap1>, FRAME_COUNT> d2dRenderTargets;
	extern ID2D1RenderTarget* g_d2dCurrentRenderTarget;

	// DWRITE
	extern ComPtr<IDWriteFactory5> dwriteFactory;

#ifdef _IMGUI
	// IMGUI
	extern ComPtr<ID3D12DescriptorHeap> imGuiSrvDescHeap;
#endif
}
