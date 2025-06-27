#pragma once

namespace Graphics::D3D
{
	class Descriptor;
	class RenderTarget;

	class SwapChain
	{
	private:
		struct FrameResource
		{
			ComPtr<ID3D12Resource> backBuffer;
#ifdef _DIRECT2D
			ComPtr<ID3D11Resource> wrappedBackBuffer;
			ComPtr<ID2D1Bitmap1> d2dRenderTarget;
#endif
			ComPtr<ID3D12CommandAllocator> commandAllocator;
			Descriptor* rtvDesc;
			UINT64 fenceValue;
		};

	public:
		SwapChain(UINT width, UINT height);
		~SwapChain();

		void OnResize(int width, int height);

		void Begin3D();
		void End3D();
#ifdef _DIRECT2D
		void Begin2D();
		void End2D();
#endif
		void Present();

		void PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget);
		void PopRenderTarget();

		Int2 GetSize() const;

	private:
		void CreateRenderTargetView();
#ifdef _DIRECT2D
		void CreateWrappedResource();
		void CreateDirect2DRenderTarget();
#endif
		void CreateDepthStencil();
		void CreateCommandAllocators();
		void CreateFence();

		void WaitForGPU();
		void WaitForPreviousFrame();

	public:
		static constexpr auto FrameCount{ 2U };

	private:
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		Int2 m_size;

		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_depthStencil;
		Descriptor* m_dsvDesc;

		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fenceEvent;
		UINT m_frameIndex;

		std::array<FrameResource, FrameCount> m_frameResources;
	};
}
