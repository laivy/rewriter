#pragma once

class Timer;

class NytApp : public TSingleton<NytApp>
{
public:
	NytApp();
	~NytApp() = default;

	void OnCreate();

	void Run();

	HWND GetHwnd() const;
	INT2 GetWindowSize() const;
	ID3D12Device* GetD3DDevice() const;
	ID3D12GraphicsCommandList* GetCommandList() const;
	ID3D12RootSignature* GetRootSignature() const;
	ID2D1DeviceContext2* GetD2DContext() const;
	IDWriteFactory5* GetDwriteFactory() const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnDestroy();
	void OnResize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT InitWnd();
	HRESULT InitDirectX();
	void CreateFactory();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateD3D11On12Device();
	void CreateD2DFactory();
	void CreateD2DDevice();
	void CreateSwapChain();
	void CreateRtvDsvDescriptorHeap();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateRootSignature();
	void CreateCommandList();
	void CreateFence();

	void Update();
	void Render();

	void ResetCommandList();
	void ExecuteCommandList();

	void WaitPrevFrame();
	void WaitForGPU();

private:
	static constexpr UINT FrameCount = 3;

	// Window
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	INT2 m_size;

	// DirectX12
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<IDXGISwapChain3>	m_swapChain;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocators;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12Resource> m_depthStencil;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12Fence> m_fence;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValues[FrameCount];
	UINT m_rtvDescriptorSize;

	// Direct3D11on12
	ComPtr<ID3D11On12Device> m_d3d11On12Device;
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	ComPtr<ID3D11Resource> m_wrappedBackBuffers[FrameCount];

	// Direct2D
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1Device2> m_d2dDevice;
	ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
	ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[FrameCount];

	// DirectWrite
	ComPtr<IDWriteFactory5>	m_dwriteFactory;

	// 게임프레임워크
	std::unique_ptr<Timer> m_timer;
};