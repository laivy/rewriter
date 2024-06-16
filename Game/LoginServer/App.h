#pragma once

class App : public TSingleton<App>
{
public:
	App();
	~App();

	void Run();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitWindow();
	void InitDirectX();
	void InitImgui();
	void InitApp();

	void CreateFactory();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateRtvDescriptorHeap();
	void CreateSrvDescriptorHeap();
	void CreateRenderTargetView();
	void CreateCommandList();
	void CreateFence();
	void WaitPrevFrame();

	void OnResize(const INT2& size);

	void Update();
	void Render();

	void RenderBackgroundWindow();

public:
	static inline auto OnPacket{ std::make_shared<Event<size_t, std::shared_ptr<Packet>>>() };

private:
	// Window
	HWND m_hWnd;
	INT2 m_size;

	// DirectX12
	static constexpr UINT FRAME_COUNT = 3;
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<IDXGISwapChain3>	m_swapChain;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[FRAME_COUNT];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_srvDescHeap;
	ComPtr<ID3D12Fence> m_fence;
	UINT m_cbvSrvUavDescriptorIncrementSize;
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValues[FRAME_COUNT];
	UINT m_rtvDescriptorSize;
	bool m_isDxInit;
};