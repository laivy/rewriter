#pragma once

class Timer;

class App : public TSingleton<App>
{
public:
	App();
	~App() = default;

	bool OnCreate();

	void Run();

	HWND GetHwnd() const;
	INT2 GetWindowSize() const;
	INT2 GetCursorPosition() const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnDestroy();
	void OnResize(int width, int height);
	void OnMouseMove(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnRButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	HRESULT InitWindow();

	void Update();
	void Render();

private:
	// Window
	bool m_isActive;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	INT2 m_size;

	// Direct3D 12
	static constexpr UINT FRAME_COUNT = 3;
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<IDXGISwapChain3>	m_swapChain;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[FRAME_COUNT];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12Resource> m_renderTargets[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
	ComPtr<ID3D12Resource> m_depthStencil;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12Fence> m_fence;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	UINT64 m_fenceValues[FRAME_COUNT];
	UINT m_rtvDescriptorSize;

	// Direct3D11on12
	ComPtr<ID3D11On12Device> m_d3d11On12Device;
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	ComPtr<ID3D11Resource> m_wrappedBackBuffers[FRAME_COUNT];

	// Direct2D
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1Device2> m_d2dDevice;
	ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[FRAME_COUNT];

	// DirectWrite
	ComPtr<IDWriteFactory5>	m_dwriteFactory;

	std::unique_ptr<Timer> m_timer;
};