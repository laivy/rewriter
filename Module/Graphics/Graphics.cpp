#include "Stdafx.h"
#include "Global.h"
#include "Graphics.h"

namespace Graphics
{
	static bool CreateDXGIFactory()
	{
		UINT flags{ 0 };
#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		if (FAILED(::CreateDXGIFactory2(flags, IID_PPV_ARGS(&dxgiFactory))))
			return false;
		if (FAILED(dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
			return false;
		return true;
	}

	static bool CreateD3DDevice()
	{
		ComPtr<IDXGIAdapter1> adapter;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			adapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;
			if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
				break;
		}
		if (!d3dDevice)
		{
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			if (FAILED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
				return false;
		}
		cbvSrvUavDescriptorIncrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return true;
	}

	static bool CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))))
			return false;
		return true;
	}

	static bool CreateSwapChain()
	{
		RECT rect{};
		::GetClientRect(hWnd, &rect);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = FRAME_COUNT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1)))
			return false;
		if (FAILED(swapChain1.As(&swapChain)))
			return false;
		frameIndex = swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	static bool CreateRtvDsvSrvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvDescHeap))))
			return false;

		rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvDescHeap))))
			return false;

		/*
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescHeap))))
			return false;
		*/

		return true;
	}

	static bool CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]))))
				return false;
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
		return true;
	}

	static bool CreateDepthStencilView(HWND hWnd)
	{
		RECT rect{};
		::GetClientRect(hWnd, &rect);

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = rect.right - rect.left;
		desc.Height = rect.bottom - rect.top;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES prop{ D3D12_HEAP_TYPE_DEFAULT };
		if (FAILED(d3dDevice->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&depthStencil))))
			return false;

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
		d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, dsvDescHeap->GetCPUDescriptorHandleForHeapStart());
		return true;
	}

	static bool CreateRootSignature()
	{
		/*
		* Common.hlsli 파일에 선언된 것과 동일해야 함
		* 0 : cbuffer cbGameObject : register(b0)
		* 1 : cbuffer cbCamera : register(b1)
		* 2 : cbuffer cbTexture : register(b2)
		* 3 : cbuffer cbLine : register(b3)
		* 4 : Texture2D g_texture : register(t0)
		*/

		std::array<CD3DX12_DESCRIPTOR_RANGE, 1> ranges{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

		std::array<CD3DX12_ROOT_PARAMETER, 5> rootParameters{};
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);
		rootParameters[2].InitAsConstantBufferView(2);
		rootParameters[3].InitAsConstantBufferView(3);
		rootParameters[4].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

		std::array<CD3DX12_STATIC_SAMPLER_DESC, 1> samplerDesc{};
		samplerDesc[0].Init(
			0,
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.0f,
			1,
			D3D12_COMPARISON_FUNC_ALWAYS,
			D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
			0.0f,
			D3D12_FLOAT32_MAX,
			D3D12_SHADER_VISIBILITY_PIXEL,
			0
		);

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Init(
			static_cast<UINT>(rootParameters.size()),
			rootParameters.data(),
			static_cast<UINT>(samplerDesc.size()),
			samplerDesc.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> signature, error;
		if (FAILED(::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
			return false;
		if (FAILED(d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
			return false;
		return true;
	}

	static bool CreateCommandList()
	{
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]))))
				return false;
		}
		if (FAILED(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList))))
			return false;
		if (FAILED(commandList->Close()))
			return false;
		return true;
	}

	static bool CreateFence()
	{
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
			return false;
		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!fenceEvent)
			return false;
		++fenceValues[frameIndex];
		return true;
	}

	static bool CreateD3D11On12Device()
	{
		ComPtr<ID3D11Device> d3d11Device;
		if (FAILED(::D3D11On12CreateDevice(
			d3dDevice.Get(),
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			reinterpret_cast<IUnknown**>(commandQueue.GetAddressOf()),
			1,
			0,
			&d3d11Device,
			&d3d11DeviceContext,
			nullptr
		)))
			return false;
		if (FAILED(d3d11Device.As(&d3d11On12Device)))
			return false;
		return true;
	}

	static bool CreateWrappedResource()
	{
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			D3D11_RESOURCE_FLAGS flags{ D3D11_BIND_RENDER_TARGET };
			if (FAILED(d3d11On12Device->CreateWrappedResource(renderTargets[i].Get(), &flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&wrappedBackBuffers[i]))))
				return false;
		}
		return true;
	}

	static bool CreateD2DFactory()
	{
		if (FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
			return false;
		return true;
	}

	static bool CreateD2DDevice()
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		if (FAILED(d3d11On12Device.As(&dxgiDevice)))
			return false;
		if (FAILED(d2dFactory->CreateDevice(dxgiDevice.Get(), &d2dDevice)))
			return false;
		if (FAILED(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext)))
			return false;
		return true;
	}

	static bool CreateD2DRenderTarget(HWND hWnd)
	{
		UINT dpi{ ::GetDpiForWindow(hWnd) };
		auto bitmapProperties{ D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			static_cast<float>(dpi),
			static_cast<float>(dpi)
		) };

		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			ComPtr<IDXGISurface> surface;
			if (FAILED(wrappedBackBuffers[i].As(&surface)))
				return false;
			if (FAILED(d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, &d2dRenderTargets[i])))
				return false;
		}

		d2dContext->SetTarget(d2dRenderTargets.front().Get());
		return true;
	}

	static bool CreateDWriteFactory()
	{
		if (FAILED(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), &dwriteFactory)))
			return false;
		return true;
	}

#ifdef _IMGUI
	static bool InitializeImGui()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&imGuiSrvDescHeap))))
			return false;

		::IMGUI_CHECKVERSION();
		::ImGui::CreateContext();
		::ImGui::GetIO().ConfigFlags |= (ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable);

		if (!::ImGui_ImplWin32_Init(hWnd))
			return false;

		if (!::ImGui_ImplDX12_Init(
			d3dDevice.Get(),
			FRAME_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			imGuiSrvDescHeap.Get(),
			imGuiSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			imGuiSrvDescHeap->GetGPUDescriptorHandleForHeapStart()
		))
			return false;

		return true;
	}

	static void CleanUpImGui()
	{
		::ImGui_ImplDX12_Shutdown();
		::ImGui_ImplWin32_Shutdown();
		::ImGui::DestroyContext();
	}
#endif // _IMGUI

	static bool ResetCommandList()
	{
		if (FAILED(commandAllocators[frameIndex]->Reset()))
			return false;
		if (FAILED(commandList->Reset(commandAllocators[frameIndex].Get(), nullptr)))
			return false;
		return true;
	}

	static bool ExecuteCommandList()
	{
		if (FAILED(commandList->Close()))
			return false;

		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
		return true;
	}

	static bool WaitPrevFrame()
	{
		const UINT64 fenceValue{ fenceValues[frameIndex] };
		if (FAILED(commandQueue->Signal(fence.Get(), fenceValue)))
			return false;

		if (fence->GetCompletedValue() < fenceValue)
		{
			if (FAILED(fence->SetEventOnCompletion(fenceValue, fenceEvent)))
				return false;
			if (::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
				return false;
		}

		frameIndex = swapChain->GetCurrentBackBufferIndex();
		fenceValues[frameIndex] = fenceValue + 1;
		return true;
	}

	static bool WaitForGPU()
	{
		if (FAILED(commandQueue->Signal(fence.Get(), fenceValues[frameIndex])))
			return false;
		if (FAILED(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent)))
			return false;
		if (::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
			return false;
		++fenceValues[frameIndex];
		return true;
	}

	DLL_API bool Initialize(HWND hWnd)
	{
		Graphics::hWnd = hWnd;

		// D3D12
		if (!CreateDXGIFactory())
			return false;
		if (!CreateD3DDevice())
			return false;
		if (!CreateCommandQueue())
			return false;
		if (!CreateSwapChain())
			return false;
		if (!CreateRtvDsvSrvDescriptorHeap())
			return false;
		if (!CreateRenderTargetView())
			return false;
		if (!CreateDepthStencilView(hWnd))
			return false;
		if (!CreateRootSignature())
			return false;
		if (!CreateCommandList())
			return false;
		if (!CreateFence())
			return false;

		// D3D11on12
		if (!CreateD3D11On12Device())
			return false;
		if (!CreateWrappedResource())
			return false;

		// D2D
		if (!CreateD2DFactory())
			return false;
		if (!CreateD2DDevice())
			return false;
		if (!CreateD2DRenderTarget(hWnd))
			return false;

		// DWRITE
		if (!CreateDWriteFactory())
			return false;

#ifdef _IMGUI
		if (!InitializeImGui())
			return false;
#endif // _IMGUI

		if (!ResetCommandList())
			return false;
		if (!ExecuteCommandList())
			return false;
		if (!WaitForGPU())
			return false;
		return true;
	}

	DLL_API bool Present()
	{
		if (FAILED(swapChain->Present(1, 0)))
			return false;

		WaitPrevFrame();
		return true;
	}

	DLL_API bool CleanUp()
	{
#ifdef _IMGUI
		CleanUpImGui();
#endif // _IMGUI
		if (!WaitForGPU())
			return false;
		if (!::CloseHandle(fenceEvent))
			return false;
#ifdef _DEBUG
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(::DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL);
#endif // _DEBUG
		return true;
	}

	DLL_API void OnResize(int width, int height)
	{
		WaitPrevFrame();
		viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };

		// 렌더 타겟 해제
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			renderTargets[i].Reset();
			wrappedBackBuffers[i].Reset();
			d2dRenderTargets[i].Reset();
			fenceValues[i] = fenceValues[frameIndex];
		}
		d2dContext->SetTarget(nullptr);
		d2dContext->Flush();
		d3d11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		d3d11DeviceContext->Flush();

		// 렌더 타겟 재생성
		DXGI_SWAP_CHAIN_DESC desc{};
		swapChain->GetDesc(&desc);
		swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}

		CreateRenderTargetView();
		CreateDepthStencilView(hWnd);
		CreateWrappedResource();
		CreateD2DRenderTarget(hWnd);
	}
}
