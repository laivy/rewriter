#pragma once

namespace Renderer3D
{
	constexpr auto FRAME_COUNT{ 3 };

	// D3D12
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
	extern ComPtr<ID3D12RootSignature> rootSignature;
	extern ComPtr<ID3D12Fence> fence;
	extern D3D12_VIEWPORT viewport;
	extern D3D12_RECT scissorRect;
	extern UINT frameIndex;
	extern HANDLE fenceEvent;
	extern UINT64 fenceValues[FRAME_COUNT];
	extern UINT rtvDescriptorSize;

	// D3D11on12
	extern ComPtr<ID3D11On12Device> d3d11On12Device;
	extern ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	extern ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];

	void Init();
	void RenderStart();
	void RenderEnd();
	void Present();
	void CleanUp();

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position = {}, float opacity = 1.0f);
}