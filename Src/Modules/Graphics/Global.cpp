#include "Stdafx.h"
#include "Camera.h"
#include "Global.h"
#include "SwapChain.h"

namespace Graphics
{
	// Windows
	HWND g_hWnd;

	// D3D12
	ComPtr<IDXGIFactory4> g_dxgiFactory;
	ComPtr<ID3D12Device> g_d3dDevice;
	ComPtr<ID3D12CommandQueue> g_commandQueue;
	std::unique_ptr<D3D::SwapChain> g_swapChain;
	ComPtr<ID3D12GraphicsCommandList> g_commandList;
	ComPtr<ID3D12Resource> g_depthStencil;
	ComPtr<ID3D12RootSignature> g_rootSignature;
	D3D12_VIEWPORT g_viewport;
	D3D12_RECT g_scissorRect;

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

#ifdef _IMGUI
	// IMGUI
	ComPtr<ID3D12DescriptorHeap> g_imGuiSrvDescHeap;
#endif

	// 로직에 사용되는 변수들
	std::vector<ComPtr<ID3D12Resource>> g_uploadBuffers;
	std::shared_ptr<D3D::Camera> g_camera;
	Int2 g_renderTargetSize{};
}
