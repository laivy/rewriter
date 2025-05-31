#pragma once

namespace Graphics
{
	namespace D3D
	{
		class Camera;
		class DescriptorHeap;
		class SwapChain;
	}

	// Windows
	extern HWND g_hWnd;

	// D3D12
	extern ComPtr<IDXGIFactory4> g_dxgiFactory;
	extern ComPtr<ID3D12Device> g_d3dDevice;
	extern ComPtr<ID3D12CommandQueue> g_commandQueue;
	extern ComPtr<ID3D12CommandAllocator> g_uploadCommandAllocator;
	extern ComPtr<ID3D12GraphicsCommandList> g_uploadCommandList;
	extern std::unique_ptr<D3D::SwapChain> g_swapChain;
	extern ComPtr<ID3D12GraphicsCommandList> g_commandList;
	extern ComPtr<ID3D12RootSignature> g_rootSignature;

#ifdef _DIRECT2D
	// D3D11on12
	extern ComPtr<ID3D11On12Device> g_d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> g_d3d11DeviceContext;

	// D2D
	extern ComPtr<ID2D1DeviceContext2> g_d2dContext;
	extern ComPtr<ID2D1Factory3> g_d2dFactory;
	extern ComPtr<ID2D1Device2> g_d2dDevice;
	extern std::vector<ID2D1RenderTarget*> g_d2dCurrentRenderTargets;
	extern ComPtr<IDWriteFactory5> g_dwriteFactory;
#endif

	// 로직에 사용되는 변수들
	extern std::vector<ComPtr<ID3D12Resource>> g_uploadBuffers;
	extern std::shared_ptr<D3D::Camera> g_camera;
}
