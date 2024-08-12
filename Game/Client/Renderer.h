#pragma once

namespace Renderer
{
	constexpr auto FRAME_COUNT{ 3u };

	// Direct3D 12
	extern ComPtr<IDXGIFactory4> factory;
	extern ComPtr<ID3D12Device> d3dDevice;
	extern ComPtr<IDXGISwapChain3>	swapChain;
	extern ComPtr<ID3D12CommandQueue> commandQueue;
	extern ComPtr<ID3D12CommandAllocator> commandAllocators[FRAME_COUNT];
	extern ComPtr<ID3D12GraphicsCommandList> commandList;
	extern ComPtr<ID3D12DescriptorHeap> rtvHeap;
	extern ComPtr<ID3D12Resource> renderTargets[FRAME_COUNT];
	extern ComPtr<ID3D12DescriptorHeap> dsvHeap;
	extern ComPtr<ID3D12Resource> depthStencil;
	extern ComPtr<ID3D12DescriptorHeap> srvHeap;
	extern ComPtr<ID3D12RootSignature> rootSignature;
	extern ComPtr<ID3D12Fence> fence;
	extern D3D12_VIEWPORT viewport;
	extern D3D12_RECT scissorRect;
	extern UINT frameIndex;
	extern HANDLE fenceEvent;
	extern UINT64 fenceValues[FRAME_COUNT];
	extern UINT rtvDescriptorSize;
	extern UINT cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	extern ComPtr<ID3D11On12Device> d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	extern ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];

	// Direct2D
	extern ComPtr<ID2D1DeviceContext2> ctx;
	extern ComPtr<ID2D1Factory3> d2dFactory;
	extern ComPtr<ID2D1Device2> d2dDevice;
	extern ComPtr<ID2D1Bitmap1> d2dRenderTargets[FRAME_COUNT];
	extern ComPtr<IDWriteFactory5> dwriteFactory;

	void Init();
	void Present();
	void CleanUp();
}