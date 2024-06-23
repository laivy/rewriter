#include "Stdafx.h"
#include "App.h"
#include "Renderer.h"

namespace
{
	void CreateFactory()
	{
		UINT dxgiFactoryFlags{ 0 };
#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&Renderer::factory)));
	}

	void CreateDevice()
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != Renderer::factory->EnumAdapters1(i, &hardwareAdapter); ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			hardwareAdapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Renderer::d3dDevice)))) break;
		}
		if (!Renderer::d3dDevice)
		{
			Renderer::factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter));
			DX::ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Renderer::d3dDevice)));
		}
		Renderer::cbvSrvUavDescriptorIncrementSize = Renderer::d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&Renderer::commandQueue)));
	}

	void CreateD3D11On12Device()
	{
		// Create an 11 device wrapped around the 12 device and share 12's command queue.
		ComPtr<ID3D11Device> d3d11Device;
		DX::ThrowIfFailed(D3D11On12CreateDevice(
			Renderer::d3dDevice.Get(),
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			nullptr,
			0,
			reinterpret_cast<IUnknown**>(Renderer::commandQueue.GetAddressOf()),
			1,
			0,
			&d3d11Device,
			&Renderer::d3d11DeviceContext,
			nullptr
		));

		// Query the 11On12 device from the 11 device.
		DX::ThrowIfFailed(d3d11Device.As(&Renderer::d3d11On12Device));
	}

	void CreateD2DDevice()
	{
		// 팩토리 생성
		DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, Renderer::d2dFactory.GetAddressOf()));

		// 디바이스 생성
		ComPtr<IDXGIDevice> dxgiDevice;
		DX::ThrowIfFailed(Renderer::d3d11On12Device.As(&dxgiDevice));
		DX::ThrowIfFailed(Renderer::d2dFactory->CreateDevice(dxgiDevice.Get(), &Renderer::d2dDevice));
		DX::ThrowIfFailed(Renderer::d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &Renderer::ctx));
		DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &Renderer::dwriteFactory));
	}

	void CreateSwapChain()
	{
		HWND hWnd{ App::hWnd };
		RECT rect{};
		::GetClientRect(hWnd, &rect);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.BufferCount = Renderer::FRAME_COUNT;
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain1;
		DX::ThrowIfFailed(Renderer::factory->CreateSwapChainForHwnd(
			Renderer::commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		));

		DX::ThrowIfFailed(swapChain1.As(&Renderer::swapChain));
		Renderer::frameIndex = Renderer::swapChain->GetCurrentBackBufferIndex();

		// ALT + ENTER 금지
		DX::ThrowIfFailed(Renderer::factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	}

	void CreateRtvDsvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = Renderer::FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = NULL;
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&Renderer::rtvHeap)));
		Renderer::rtvDescriptorSize = Renderer::d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = NULL;
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&Renderer::dsvHeap)));
	}

	void CreateRenderTargetView()
	{
		UINT dpi{ ::GetDpiForWindow(App::hWnd) };
		D2D1_BITMAP_PROPERTIES1 bitmapProperties{ D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			static_cast<float>(dpi),
			static_cast<float>(dpi)
		) };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ Renderer::rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < Renderer::FRAME_COUNT; ++i)
		{
			DX::ThrowIfFailed(Renderer::swapChain->GetBuffer(i, IID_PPV_ARGS(&Renderer::renderTargets[i])));
			Renderer::d3dDevice->CreateRenderTargetView(Renderer::renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(Renderer::rtvDescriptorSize);

			D3D11_RESOURCE_FLAGS d3d11Flags{ D3D11_BIND_RENDER_TARGET };
			DX::ThrowIfFailed(Renderer::d3d11On12Device->CreateWrappedResource(
				Renderer::renderTargets[i].Get(),
				&d3d11Flags,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PRESENT,
				IID_PPV_ARGS(&Renderer::wrappedBackBuffers[i])
			));

			ComPtr<IDXGISurface> surface;
			DX::ThrowIfFailed(Renderer::wrappedBackBuffers[i].As(&surface));
			DX::ThrowIfFailed(Renderer::ctx->CreateBitmapFromDxgiSurface(
				surface.Get(),
				&bitmapProperties,
				&Renderer::d2dRenderTargets[i]
			));

			DX::ThrowIfFailed(Renderer::d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Renderer::commandAllocators[i])));
		}
	}

	void CreateSrvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = NULL;
		Renderer::d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&Renderer::srvHeap));
	}

	void CreateDepthStencilView()
	{
		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = App::size.x;
		desc.Height = App::size.y;
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

		DX::ThrowIfFailed(Renderer::d3dDevice->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&Renderer::depthStencil)
		));

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
		Renderer::d3dDevice->CreateDepthStencilView(Renderer::depthStencil.Get(), &depthStencilViewDesc, Renderer::dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void CreateRootSignature()
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

		std::array<CD3DX12_ROOT_PARAMETER, 5> rootParameter{};
		rootParameter[0].InitAsConstantBufferView(0);
		rootParameter[1].InitAsConstantBufferView(1);
		rootParameter[2].InitAsConstantBufferView(2);
		rootParameter[3].InitAsConstantBufferView(3);
		rootParameter[4].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

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
		rootSignatureDesc.Init(static_cast<UINT>(rootParameter.size()), rootParameter.data(), static_cast<UINT>(samplerDesc.size()), samplerDesc.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature, error;
		DX::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&Renderer::rootSignature)));
	}

	void CreateCommandList()
	{
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, Renderer::commandAllocators[Renderer::frameIndex].Get(), nullptr, IID_PPV_ARGS(&Renderer::commandList)));
		DX::ThrowIfFailed(Renderer::commandList->Close());
	}

	void CreateFence()
	{
		DX::ThrowIfFailed(Renderer::d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Renderer::fence)));
		Renderer::fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		++Renderer::fenceValues[Renderer::frameIndex];
	}

	void ResetCommandList()
	{
		DX::ThrowIfFailed(Renderer::commandAllocators[Renderer::frameIndex]->Reset());
		DX::ThrowIfFailed(Renderer::commandList->Reset(Renderer::commandAllocators[Renderer::frameIndex].Get(), nullptr));
	}

	void ExecuteCommandList()
	{
		DX::ThrowIfFailed(Renderer::commandList->Close());
		ID3D12CommandList* ppCommandList[]{ Renderer::commandList.Get() };
		Renderer::commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void WaitPrevFrame()
	{
		const UINT64 currentFenceValue{ Renderer::fenceValues[Renderer::frameIndex] };
		DX::ThrowIfFailed(Renderer::commandQueue->Signal(Renderer::fence.Get(), currentFenceValue));

		if (Renderer::fence->GetCompletedValue() < Renderer::fenceValues[Renderer::frameIndex])
		{
			DX::ThrowIfFailed(Renderer::fence->SetEventOnCompletion(Renderer::fenceValues[Renderer::frameIndex], Renderer::fenceEvent));
			WaitForSingleObjectEx(Renderer::fenceEvent, INFINITE, FALSE);
		}

		Renderer::frameIndex = Renderer::swapChain->GetCurrentBackBufferIndex();
		Renderer::fenceValues[Renderer::frameIndex] = currentFenceValue + 1;
	}

	void WaitForGPU()
	{
		DX::ThrowIfFailed(Renderer::commandQueue->Signal(Renderer::fence.Get(), Renderer::fenceValues[Renderer::frameIndex]));
		DX::ThrowIfFailed(Renderer::fence->SetEventOnCompletion(Renderer::fenceValues[Renderer::frameIndex], Renderer::fenceEvent));
		WaitForSingleObjectEx(Renderer::fenceEvent, INFINITE, FALSE);
		++Renderer::fenceValues[Renderer::frameIndex];
	}
}

namespace Renderer
{
	// Direct3D 12
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> d3dDevice;
	ComPtr<IDXGISwapChain3>	swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocators[FRAME_COUNT];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	ComPtr<ID3D12Resource> depthStencil;
	ComPtr<ID3D12DescriptorHeap> srvHeap;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12Fence> fence;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	UINT frameIndex;
	HANDLE fenceEvent;
	UINT64 fenceValues[FRAME_COUNT];
	UINT rtvDescriptorSize;
	UINT cbvSrvUavDescriptorIncrementSize;

	// D3D11on12
	ComPtr<ID3D11On12Device> d3d11On12Device;
	ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];

	// Direct2D
	ComPtr<ID2D1DeviceContext2> ctx;
	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device2> d2dDevice;
	ComPtr<ID2D1Bitmap1> d2dRenderTargets[FRAME_COUNT];
	ComPtr<IDWriteFactory5> dwriteFactory;

	void OnResize(int width, int height)
	{
		viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };

		WaitPrevFrame();

		// 렌더 타겟 해제
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			renderTargets[i].Reset();
			wrappedBackBuffers[i].Reset();
			d2dRenderTargets[i].Reset();
			fenceValues[i] = fenceValues[frameIndex];
		}

		ctx->SetTarget(nullptr);
		ctx->Flush();

		d3d11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		d3d11DeviceContext->Flush();

		DXGI_SWAP_CHAIN_DESC desc{};
		swapChain->GetDesc(&desc);
		swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		// 렌더 타겟 재생성
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}

		CreateRenderTargetView();
		CreateDepthStencilView();
	}

	void Init()
	{
		CreateFactory();
		CreateDevice();
		CreateCommandQueue();
		CreateD3D11On12Device();
		CreateD2DDevice();
		CreateSwapChain();
		CreateRtvDsvDescriptorHeap();
		CreateRenderTargetView();
		CreateDepthStencilView();
		CreateSrvDescriptorHeap();
		CreateRootSignature();
		CreateCommandList();
		CreateFence();

		ResetCommandList();
		ExecuteCommandList();
		WaitForGPU();

		App::OnResize->Register(&OnResize);
	}

	void Present()
	{
		DX::ThrowIfFailed(swapChain->Present(1, NULL));
		WaitPrevFrame();
	}

	void CleanUp()
	{
		WaitForGPU();
		::CloseHandle(fenceEvent);

#ifdef _DEBUG
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(NULL, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL);
		}
#endif
	}
}