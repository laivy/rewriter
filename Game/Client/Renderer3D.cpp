#include "Stdafx.h"
#include "App.h"
#include "Renderer3D.h"

namespace
{
	using namespace Renderer3D;

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
		DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
	}

	void CreateDevice()
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		for (UINT i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &hardwareAdapter); ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			hardwareAdapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice)))) break;
		}
		if (!d3dDevice)
		{
			factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter));
			DX::ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice)));
		}
		g_cbvSrvUavDescriptorIncrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		DX::ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));
	}

	void CreateSwapChain()
	{
		HWND hWnd{ App::GetInstance()->GetHwnd() };
		RECT rect{};
		::GetClientRect(hWnd, &rect);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.BufferCount = FRAME_COUNT;
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain1;
		DX::ThrowIfFailed(factory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		));

		DX::ThrowIfFailed(swapChain1.As(&swapChain));
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		// ALT + ENTER 금지
		DX::ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	}

	void CreateRtvDsvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = NULL;
		DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
		rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = NULL;
		DX::ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
	}

	void CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			DX::ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);

			DX::ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
		}
	}

	void CreateDepthStencilView()
	{
		auto size{ App::GetInstance()->GetWindowSize() };

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = size.x;
		desc.Height = size.y;
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

		DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearValue,
			IID_PPV_ARGS(&depthStencil)
		));

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
		d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void CreateRootSignature()
	{
		std::array<CD3DX12_DESCRIPTOR_RANGE, 1> ranges{};
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

		std::array<CD3DX12_ROOT_PARAMETER, static_cast<UINT>(RootParamIndex::COUNT)> rootParameter{};
		rootParameter[static_cast<UINT>(RootParamIndex::GAMEOBJECT)].InitAsConstantBufferView(static_cast<UINT>(RootParamIndex::GAMEOBJECT));
		rootParameter[static_cast<UINT>(RootParamIndex::CAMERA)].InitAsConstantBufferView(static_cast<UINT>(RootParamIndex::CAMERA));
		rootParameter[static_cast<UINT>(RootParamIndex::TEXTURE)].InitAsConstantBufferView(static_cast<UINT>(RootParamIndex::TEXTURE));
#ifdef _DEBUG
		rootParameter[static_cast<UINT>(RootParamIndex::LINE)].InitAsConstantBufferView(static_cast<UINT>(RootParamIndex::LINE));
#endif
		rootParameter[static_cast<UINT>(RootParamIndex::TEXTURE0)].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

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
		DX::ThrowIfFailed(d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	}

	void CreateCommandList()
	{
		DX::ThrowIfFailed(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList)));
		DX::ThrowIfFailed(commandList->Close());
	}

	void CreateFence()
	{
		DX::ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		++fenceValues[frameIndex];
	}

	void ResetCommandList()
	{
		DX::ThrowIfFailed(commandAllocators[frameIndex]->Reset());
		DX::ThrowIfFailed(commandList->Reset(commandAllocators[frameIndex].Get(), nullptr));
	}

	void ExecuteCommandList()
	{
		DX::ThrowIfFailed(commandList->Close());
		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void WaitPrevFrame()
	{
		const UINT64 currentFenceValue{ fenceValues[frameIndex] };
		DX::ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFenceValue));

		if (fence->GetCompletedValue() < fenceValues[frameIndex])
		{
			DX::ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
			WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		}

		frameIndex = swapChain->GetCurrentBackBufferIndex();
		fenceValues[frameIndex] = currentFenceValue + 1;
	}

	void WaitForGPU()
	{
		DX::ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValues[frameIndex]));
		DX::ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent));
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		++fenceValues[frameIndex];
	}
}

namespace Renderer3D
{
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
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12Fence> fence;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	UINT frameIndex;
	HANDLE fenceEvent;
	UINT64 fenceValues[FRAME_COUNT];
	UINT rtvDescriptorSize;

	ComPtr<ID3D11On12Device> d3d11On12Device;
	ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
	ComPtr<ID3D11Resource> wrappedBackBuffers[FRAME_COUNT];

	void Init()
	{
		CreateFactory();
		CreateDevice();
		CreateCommandQueue();
		CreateSwapChain();
		CreateRtvDsvDescriptorHeap();
		CreateRenderTargetView();
		CreateDepthStencilView();
		CreateRootSignature();
		CreateCommandList();
		CreateFence();

		ResetCommandList();
		ExecuteCommandList();
		WaitForGPU();
	}

	void RenderStart()
	{
		ResetCommandList();

		commandList->SetGraphicsRootSignature(rootSignature.Get());
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(frameIndex), rtvDescriptorSize };
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvHeap->GetCPUDescriptorHandleForHeapStart() };
		commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);

		constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

		//if (auto srvDescHeap{ ResourceManager::GetInstance()->GetSrvDescriptorHeap() })
		//	m_commandList->SetDescriptorHeaps(1, srvDescHeap);
	}

	void RenderEnd()
	{
		ExecuteCommandList();
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
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL);
		}
#endif
	}

	void DrawImage(const std::shared_ptr<Resource::Image>& image, const INT2& position, float opacity)
	{
		
	}
}