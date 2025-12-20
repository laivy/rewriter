#include "Pch.h"
#include "SwapChain.h"

namespace Graphics
{
	SwapChain::SwapChain() :
		m_dsvHandle{},
		m_viewport{},
		m_scissorRect{},
		m_frameIndex{ 0 },
		m_frameResources{}
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx || !ctx->hWnd || !ctx->dxgiFactory || !ctx->d3d11On12Device)
			return;

		RECT rect{};
		if (!::GetClientRect(static_cast<HWND>(ctx->hWnd), &rect))
			return;

		const UINT width{ static_cast<UINT>(rect.right - rect.left) };
		const UINT height{ static_cast<UINT>(rect.bottom - rect.top) };
		m_viewport = CD3DX12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) };
		m_scissorRect = D3D12_RECT{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = Context::FrameCount;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(ctx->dxgiFactory->CreateSwapChainForHwnd(
			ctx->commandQueue.Get(),
			static_cast<HWND>(ctx->hWnd),
			&desc,
			nullptr,
			nullptr,
			&swapChain1)))
		{
			return;
		}
		if (FAILED(swapChain1.As(&m_swapChain)))
			return;

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		if (!CreateFrameResource())
			return;
		if (!CreateDepthStencil())
			return;
	}

	bool SwapChain::Bind3D(ID3D12GraphicsCommandList* commandList)
	{
		const auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(m_frameResources[m_frameIndex].backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET) };
		commandList->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_frameResources[m_frameIndex].rtvHandle.cpuHandle };
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{ m_dsvHandle.cpuHandle };
		commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
		commandList->RSSetViewports(1, &m_viewport);
		commandList->RSSetScissorRects(1, &m_scissorRect);

		constexpr std::array ClearColor{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		commandList->ClearRenderTargetView(rtvHandle, ClearColor.data(), 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		return true;
	}

	bool SwapChain::Bind2D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;

		auto& frameResource{ m_frameResources[m_frameIndex] };
		ctx->d3d11On12Device->AcquireWrappedResources(frameResource.wrappedBackBuffer.GetAddressOf(), 1);
		ctx->d2dContext->SetTarget(frameResource.d2dBitmap.Get());
		ctx->d2dContext->BeginDraw();
		return true;
	}

	bool SwapChain::Unbind2D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;
		if (FAILED(ctx->d2dContext->EndDraw()))
			return false;
		auto& frameResource{ m_frameResources[m_frameIndex] };
		ctx->d3d11On12Device->ReleaseWrappedResources(frameResource.wrappedBackBuffer.GetAddressOf(), 1);
		ctx->d3d11DeviceContext->Flush();
		return true;
	}

	bool SwapChain::Present()
	{
		if (FAILED(m_swapChain->Present(1, 0)))
			return false;
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	bool SwapChain::Resize(Int2 size)
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;

		// 리셋
		for (auto& frameResource : m_frameResources)
		{
			frameResource.backBuffer.Reset();
			frameResource.wrappedBackBuffer.Reset();
			frameResource.d2dBitmap.Reset();

			ctx->descriptorManager->Free(frameResource.rtvHandle);
			frameResource.rtvHandle = {};
		}
		ctx->d2dContext->SetTarget(nullptr);
		ctx->d2dContext->Flush();
		ctx->d3d11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		ctx->d3d11DeviceContext->Flush();

		m_viewport = CD3DX12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) };
		m_scissorRect = D3D12_RECT{ 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };

		DXGI_SWAP_CHAIN_DESC desc{};
		if (FAILED(m_swapChain->GetDesc(&desc)))
			return false;
		if (FAILED(m_swapChain->ResizeBuffers(desc.BufferCount, size.x, size.y, desc.BufferDesc.Format, desc.Flags)))
			return false;
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		if (!CreateFrameResource())
			return false;
		if (!CreateDepthStencil())
			return false;
		return true;
	}

	bool SwapChain::CreateFrameResource()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;

		for (UINT i{ 0 }; i < Context::FrameCount; ++i)
		{
			auto& frameResource{ m_frameResources.at(i) };
			if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&frameResource.backBuffer))))
				return false;

			// D3D12
			ctx->descriptorManager->Free(frameResource.rtvHandle);
			frameResource.rtvHandle = ctx->descriptorManager->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			ctx->d3d12Device->CreateRenderTargetView(
				frameResource.backBuffer.Get(),
				nullptr,
				frameResource.rtvHandle.cpuHandle
			);

			// D3D11On12
			D3D11_RESOURCE_FLAGS flags{ D3D11_BIND_RENDER_TARGET };
			if (FAILED(ctx->d3d11On12Device->CreateWrappedResource(
				frameResource.backBuffer.Get(),
				&flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&frameResource.wrappedBackBuffer))))
			{
				return false;
			}

			// D2D
			UINT dpi{ ::GetDpiForWindow(static_cast<HWND>(ctx->hWnd)) };
			auto bitmapProperties{ D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				static_cast<float>(dpi),
				static_cast<float>(dpi)
			) };

			Microsoft::WRL::ComPtr<IDXGISurface> dxgiSurface;
			if (FAILED(frameResource.wrappedBackBuffer.As(&dxgiSurface)))
				return false;
			if (FAILED(ctx->d2dContext->CreateBitmapFromDxgiSurface(
				dxgiSurface.Get(),
				&bitmapProperties,
				&frameResource.d2dBitmap)))
			{
				return false;
			}
		}

		ctx->d2dContext->SetTarget(m_frameResources.at(m_frameIndex).d2dBitmap.Get());
		return true;
	}

	bool SwapChain::CreateDepthStencil()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		if (FAILED(m_swapChain->GetDesc(&swapChainDesc)))
			return false;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = swapChainDesc.BufferDesc.Width;
		desc.Height = swapChainDesc.BufferDesc.Height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
		if (FAILED(ctx->d3d12Device->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencil))))
		{
			return false;
		}

		ctx->descriptorManager->Free(m_dsvHandle);
		m_dsvHandle = ctx->descriptorManager->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		ctx->d3d12Device->CreateDepthStencilView(
			m_depthStencil.Get(),
			&dsvDesc,
			m_dsvHandle.cpuHandle
		);
		return true;
	}
}
