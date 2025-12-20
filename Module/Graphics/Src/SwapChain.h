#pragma once
#include "Context.h"
#include "Descriptor.h"

namespace Graphics
{
	class SwapChain
	{
	private:
		struct FrameResource
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			Microsoft::WRL::ComPtr<ID3D11Resource> wrappedBackBuffer;
			Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dBitmap;
			Descriptor::Handle rtvHandle;
		};

	public:
		SwapChain();

		bool Bind3D(ID3D12GraphicsCommandList* commandList);
		bool Bind2D();
		bool Unbind2D();
		bool Present();

		bool Resize(Int2 size);

	private:
		bool CreateFrameResource();
		bool CreateDepthStencil();

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;
		Descriptor::Handle m_dsvHandle;
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissorRect;
		UINT m_frameIndex;
		std::array<FrameResource, Context::FrameCount> m_frameResources;
	};
}
