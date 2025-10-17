#include "Pch.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"
#include "RenderTarget.h"
#include "SwapChain.h"

namespace Graphics::D3D
{
	SwapChain::SwapChain(UINT width, UINT height) :
		m_viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f },
		m_scissorRect{ 0, 0, static_cast<long>(width), static_cast<long>(height) },
		m_size{ static_cast<int32_t>(width), static_cast<int32_t>(height) },
		m_dsvDesc{ nullptr },
		m_fenceEvent{ NULL },
		m_frameIndex{},
		m_frameResources{}
	{
		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = FrameCount;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(g_dxgiFactory->CreateSwapChainForHwnd(g_commandQueue.Get(), static_cast<HWND>(g_hWnd), &desc, nullptr, nullptr, &swapChain1)))
		{
			assert(false);
			return;
		}

		ComPtr<IDXGISwapChain3> swapChain3;
		if (FAILED(swapChain1.As(&swapChain3)))
		{
			assert(false);
			return;
		}

		if (FAILED(g_dxgiFactory->MakeWindowAssociation(static_cast<HWND>(g_hWnd), DXGI_MWA_NO_ALT_ENTER)))
		{
			assert(false);
			return;
		}

		m_swapChain = swapChain3;
		m_frameIndex = swapChain3->GetCurrentBackBufferIndex();

		CreateRenderTargetView();
#ifdef _DIRECT2D
		CreateWrappedResource();
		CreateDirect2DRenderTarget();
#endif
		CreateDepthStencil();
		CreateCommandAllocators();
		CreateFence();
	}

	SwapChain::~SwapChain()
	{
		WaitForGPU();
		::CloseHandle(m_fenceEvent);
	}

	void SwapChain::OnResize(int width, int height)
	{
		WaitForPreviousFrame();
		for (size_t i{ 0 }; i < m_frameResources.size(); ++i)
		{
			m_frameResources[i].backBuffer.Reset();
#ifdef _DIRECT2D
			m_frameResources[i].wrappedBackBuffer.Reset();
			m_frameResources[i].d2dRenderTarget.Reset();
#endif
			m_frameResources[i].fenceValue = m_frameResources[m_frameIndex].fenceValue;
		}

#ifdef _DIRECT2D
		g_d2dContext->SetTarget(nullptr);
		g_d2dContext->Flush();
		g_d3d11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		g_d3d11DeviceContext->Flush();
#endif

		m_viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		m_scissorRect = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
		m_size = Int2{ static_cast<int32_t>(width), static_cast<int32_t>(height) };

		DXGI_SWAP_CHAIN_DESC desc{};
		m_swapChain->GetDesc(&desc);
		if (FAILED(m_swapChain->ResizeBuffers(desc.BufferCount, width, height, desc.BufferDesc.Format, desc.Flags)))
		{
			assert(false);
			return;
		}
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		CreateRenderTargetView();
		CreateDepthStencil();
#ifdef _DIRECT2D
		CreateWrappedResource();
		CreateDirect2DRenderTarget();
#endif
	}

	void SwapChain::Begin3D()
	{
		if (FAILED(m_frameResources[m_frameIndex].commandAllocator->Reset()))
		{
			assert(false);
			return;
		}

		if (FAILED(g_commandList->Reset(m_frameResources[m_frameIndex].commandAllocator.Get(), nullptr)))
		{
			assert(false);
			return;
		}

		g_commandList->SetGraphicsRootSignature(g_rootSignature.Get());

		const auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(m_frameResources[m_frameIndex].backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET) };
		g_commandList->ResourceBarrier(1, &barrier);

		auto rtvHandle{ m_frameResources[m_frameIndex].rtvDesc->GetCpuHandle() };
		auto dsvHandle{ m_dsvDesc->GetCpuHandle() };
		g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		g_commandList->RSSetViewports(1, &m_viewport);
		g_commandList->RSSetScissorRects(1, &m_scissorRect);

		constexpr std::array clearColor{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		g_commandList->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
		g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		if (auto dm{ D3D::DescriptorManager::GetInstance() })
			dm->SetDescriptorHeaps();
	}

	void SwapChain::End3D()
	{
#ifndef _DIRECT2D
		const auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(m_frameResources[m_frameIndex].backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT) };
		g_commandList->ResourceBarrier(1, &barrier);
#endif
		if (FAILED(g_commandList->Close()))
		{
			assert(false);
			return;
		}

		std::array<ID3D12CommandList*, 1> commandLists{ g_commandList.Get() };
		g_commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
	}

#ifdef _DIRECT2D
	void SwapChain::Begin2D()
	{
		g_d3d11On12Device->AcquireWrappedResources(m_frameResources[m_frameIndex].wrappedBackBuffer.GetAddressOf(), 1);
		g_d2dContext->SetTarget(m_frameResources[m_frameIndex].d2dRenderTarget.Get());
		g_d2dContext->BeginDraw();
		g_d2dCurrentRenderTargets.clear();
		g_d2dCurrentRenderTargets.push_back(g_d2dContext.Get());
	}

	void SwapChain::End2D()
	{
		if (FAILED(g_d2dContext->EndDraw()))
		{
			assert(false);
			return;
		}

		g_d3d11On12Device->ReleaseWrappedResources(m_frameResources[m_frameIndex].wrappedBackBuffer.GetAddressOf(), 1);
		g_d3d11DeviceContext->Flush();
	}
#endif

	void SwapChain::Present()
	{
		if (FAILED(m_swapChain->Present(1, 0)))
		{
			assert(false);
			return;
		}

		WaitForPreviousFrame();
	}

	static std::vector<std::shared_ptr<RenderTarget>> g_renderTargets;
	void SwapChain::PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget)
	{
		const auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->GetResource().Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET) };
		g_commandList->ResourceBarrier(1, &barrier);

		auto rtvHandle{ renderTarget->GetRenderTargetCpuHandle() };
		auto dsvHandle{ renderTarget->GetDepthStencilCpuHandle() };
		g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		g_commandList->RSSetViewports(1, renderTarget->GetViewport());
		g_commandList->RSSetScissorRects(1, renderTarget->GetScissorRect());
		g_commandList->ClearRenderTargetView(rtvHandle, renderTarget->GetClearColor(), 0, nullptr);
		g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		g_renderTargets.push_back(renderTarget);
	}

	void SwapChain::PopRenderTarget()
	{
		const auto barrier{ CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets.back()->GetResource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON) };
		g_commandList->ResourceBarrier(1, &barrier);
		g_renderTargets.pop_back();
		if (g_renderTargets.empty())
		{
			auto rtvHandle{ m_frameResources[m_frameIndex].rtvDesc->GetCpuHandle() };
			auto dsvHandle{ m_dsvDesc->GetCpuHandle() };
			g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
			g_commandList->RSSetViewports(1, &m_viewport);
			g_commandList->RSSetScissorRects(1, &m_scissorRect);
		}
		else
		{
			const auto& renderTarget{ g_renderTargets.back() };
			auto rtvHandle{ renderTarget->GetRenderTargetCpuHandle() };
			auto dsvHandle{ renderTarget->GetDepthStencilCpuHandle() };
			g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
			g_commandList->RSSetViewports(1, renderTarget->GetViewport());
			g_commandList->RSSetScissorRects(1, renderTarget->GetScissorRect());
			g_commandList->ClearRenderTargetView(rtvHandle, renderTarget->GetClearColor(), 0, nullptr);
			g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		}
	}

	Int2 SwapChain::GetSize() const
	{
		return m_size;
	}

	void SwapChain::CreateRenderTargetView()
	{
		auto dm{ DescriptorManager::GetInstance() };
		if (!dm)
			return;

		for (size_t i{ 0 }; i < FrameCount; ++i)
		{
			if (FAILED(m_swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&m_frameResources[i].backBuffer))))
			{
				assert(false);
				return;
			}

			if (m_frameResources[i].rtvDesc)
			{
				dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_frameResources[i].rtvDesc);
				m_frameResources[i].rtvDesc = nullptr;
			}

			m_frameResources[i].rtvDesc = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_frameResources[i].rtvDesc->CreateRenderTargetView(m_frameResources[i].backBuffer, nullptr);
		}
	}

#ifdef _DIRECT2D
	void SwapChain::CreateWrappedResource()
	{
		for (size_t i{ 0 }; i < FrameCount; ++i)
		{
			D3D11_RESOURCE_FLAGS flags{ D3D11_BIND_RENDER_TARGET };
			if (FAILED(g_d3d11On12Device->CreateWrappedResource(
				m_frameResources[i].backBuffer.Get(),
				&flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&m_frameResources[i].wrappedBackBuffer))))
			{
				assert(false);
				return;
			}
		}
	}

	void SwapChain::CreateDirect2DRenderTarget()
	{
		UINT dpi{ ::GetDpiForWindow(static_cast<HWND>(g_hWnd)) };
		auto bitmapProperties{ D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			static_cast<float>(dpi),
			static_cast<float>(dpi)
		) };

		for (size_t i{ 0 }; i < FrameCount; ++i)
		{
			ComPtr<IDXGISurface> surface;
			if (FAILED(m_frameResources[i].wrappedBackBuffer.As(&surface)))
			{
				assert(false);
				return;
			}
			if (FAILED(g_d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, &m_frameResources[i].d2dRenderTarget)))
			{
				assert(false);
				return;
			}
		}

		g_d2dContext->SetTarget(m_frameResources.front().d2dRenderTarget.Get());
		g_d2dCurrentRenderTargets.push_back(g_d2dContext.Get());
	}
#endif

	void SwapChain::CreateDepthStencil()
	{
		// 깊이스텐실뷰
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		m_swapChain->GetDesc(&swapChainDesc);

		D3D12_RESOURCE_DESC bufferDesc{};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		bufferDesc.Width = swapChainDesc.BufferDesc.Width;
		bufferDesc.Height = swapChainDesc.BufferDesc.Height;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES heapProp{ D3D12_HEAP_TYPE_DEFAULT };
		if (FAILED(g_d3dDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&m_depthStencil))))
		{
			assert(false);
			return;
		}

		if (auto dm{ DescriptorManager::GetInstance()})
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			if (m_dsvDesc)
			{
				dm->Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_dsvDesc);
				m_dsvDesc = nullptr;
			}
			m_dsvDesc = dm->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_dsvDesc->CreateDepthStencilView(m_depthStencil, &dsvDesc);
		}
	}

	void SwapChain::CreateCommandAllocators()
	{
		// 명령할당자
		for (size_t i{ 0 }; i < FrameCount; ++i)
		{
			if (FAILED(g_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_frameResources[i].commandAllocator))))
			{
				assert(false);
				return;
			}
		}

		// 명령리스트
		// 0번 명령할당자를 이용하여 모든 프레임에서 공통으로 사용할 명령리스트 만듦
		if (FAILED(g_d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			m_frameResources.front().commandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(&g_commandList))))
		{
			assert(false);
			return;
		}
		if (FAILED(g_commandList->Close()))
		{
			assert(false);
			return;
		}
	}

	void SwapChain::CreateFence()
	{
		if (FAILED(g_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
		{
			assert(false);
			return;
		}

		m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_fenceEvent)
		{
			assert(false);
			return;
		}

		++m_frameResources[m_frameIndex].fenceValue;
	}

	void SwapChain::WaitForGPU()
	{
		if (FAILED(g_commandQueue->Signal(m_fence.Get(), m_frameResources[m_frameIndex].fenceValue)))
		{
			assert(false);
			return;
		}
		if (FAILED(m_fence->SetEventOnCompletion(m_frameResources[m_frameIndex].fenceValue, m_fenceEvent)))
		{
			assert(false);
			return;
		}
		if (::WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
		{
			assert(false);
			return;
		}
		++m_frameResources[m_frameIndex].fenceValue;
	}

	void SwapChain::WaitForPreviousFrame()
	{
		const UINT64 fenceValue{ m_frameResources[m_frameIndex].fenceValue };
		if (FAILED(g_commandQueue->Signal(m_fence.Get(), fenceValue)))
		{
			assert(false);
			return;
		}

		if (m_fence->GetCompletedValue() < fenceValue)
		{
			if (FAILED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent)))
			{
				assert(false);
				return;
			}
			if (::WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
			{
				assert(false);
				return;
			}
		}

		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
		m_frameResources[m_frameIndex].fenceValue = fenceValue + 1;
	}
}
