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

		void Begin3D();
		void End3D();
#ifdef _DIRECT2D
		void Begin2D();
		void End2D();
#endif
		void Present();

		void Resize(UINT width, UINT height);
		void PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget);
		void PopRenderTarget();

		void WaitForGPU();
		void WaitForPreviousFrame();

	private:
		void CreateRenderTargetView();
#ifdef _DIRECT2D
		void CreateWrappedResource();
		void CreateDirect2DRenderTarget();
#endif
		void CreateDepthStencil();
		void CreateCommandAllocators();
		void CreateFence();

	public:
		static constexpr auto FRAME_COUNT{ 2U };

	private:
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12Resource> m_depthStencil;
		Descriptor* m_dsvDesc;

		ComPtr<ID3D12Fence> m_fence;
		HANDLE m_fenceEvent;
		UINT m_frameIndex;

		std::array<FrameResource, FRAME_COUNT> m_frameResources;
	};
}
