#include "Stdafx.h"
#include "Global.h"

namespace Graphics
{
	// Windows
	HWND hWnd;

	// D3D12
	ComPtr<IDXGIFactory4> dxgiFactory;
	ComPtr<IDXGISwapChain3>	swapChain;
	ComPtr<ID3D12Device> d3dDevice;
	ComPtr<ID3D12CommandQueue> commandQueue;
	std::array<ComPtr<ID3D12CommandAllocator>, FRAME_COUNT> commandAllocators;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12DescriptorHeap> rtvDescHeap;
	ComPtr<ID3D12DescriptorHeap> dsvDescHeap;
	ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	std::array<ComPtr<ID3D12Resource>, FRAME_COUNT> renderTargets;
	ComPtr<ID3D12Resource> depthStencil;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12Fence> fence;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	HANDLE fenceEvent;
	std::array<UINT64, FRAME_COUNT> fenceValues;
	UINT frameIndex;
	UINT rtvDescriptorSize;
	UINT cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	ComPtr<ID3D11On12Device> d3d11On12Device;
	ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	std::array<ComPtr<ID3D11Resource>, FRAME_COUNT> wrappedBackBuffers;

	// D2D
	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device2> d2dDevice;
	ComPtr<ID2D1DeviceContext2> d2dContext;
	std::array<ComPtr<ID2D1Bitmap1>, FRAME_COUNT> d2dRenderTargets;

	// DWRITE
	ComPtr<IDWriteFactory5> dwriteFactory;

#ifdef _IMGUI
	// IMGUI
	ComPtr<ID3D12DescriptorHeap> imGuiSrvDescHeap;
#endif
}
