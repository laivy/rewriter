// C/C++
#include <optional>

// Windows
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// DirectX
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include "External/DirectX/d3dx12.h"

// Project
#include "ImguiEx.h"

namespace
{
	constexpr auto FRAME_COUNT{ 2u };
	struct Standalone
	{
		ComPtr<IDXGIFactory4> factory;
		ComPtr<ID3D12Device> d3dDevice;
		ComPtr<IDXGISwapChain3>	swapChain;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12CommandAllocator> commandAllocators[FRAME_COUNT];
		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		ComPtr<ID3D12Resource> renderTargets[FRAME_COUNT];
		ComPtr<ID3D12Fence> fence;
		D3D12_VIEWPORT viewport;
		UINT frameIndex;
		HANDLE fenceEvent;
		UINT64 fenceValues[FRAME_COUNT];
		UINT rtvDescriptorSize;
	};

	HWND hWnd;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	std::optional<Standalone> standalone;

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
		::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&standalone->factory));
		standalone->factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	}

	void CreateDevice()
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		for (UINT i = 0; standalone->factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			hardwareAdapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;
			::D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&standalone->d3dDevice));
			break;
		}
		if (!standalone->d3dDevice)
		{
			standalone->factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter));
			::D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&standalone->d3dDevice));
		}
	}

	void CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		standalone->d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&standalone->commandQueue));
	}

	void CreateSwapChain()
	{
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
		standalone->factory->CreateSwapChainForHwnd(
			standalone->commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		);

		swapChain1.As(&standalone->swapChain);
		standalone->frameIndex = standalone->swapChain->GetCurrentBackBufferIndex();
	}

	void CreateRtvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = NULL;
		standalone->d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&standalone->rtvHeap));
		standalone->rtvDescriptorSize = standalone->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	void CreateSrvDescriptorHeap(ID3D12Device* device)
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = NULL;
		device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescHeap));
	}

	void CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ standalone->rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			standalone->swapChain->GetBuffer(i, IID_PPV_ARGS(&standalone->renderTargets[i]));
			standalone->d3dDevice->CreateRenderTargetView(standalone->renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(standalone->rtvDescriptorSize);

			standalone->d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&standalone->commandAllocators[i]));
		}
	}

	void CreateCommandList()
	{
		standalone->d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, standalone->commandAllocators[standalone->frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList));
		commandList->Close();
	}

	void CreateFence()
	{
		standalone->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&standalone->fence));
		standalone->fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		++standalone->fenceValues[standalone->frameIndex];
	}

	void ResetCommandList()
	{
		standalone->commandAllocators[standalone->frameIndex]->Reset();
		commandList->Reset(standalone->commandAllocators[standalone->frameIndex].Get(), nullptr);
	}

	void ExecuteCommandList()
	{
		commandList->Close();
		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		standalone->commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void WaitPrevFrame()
	{
		const UINT64 currentFenceValue{ standalone->fenceValues[standalone->frameIndex] };
		standalone->commandQueue->Signal(standalone->fence.Get(), currentFenceValue);

		if (standalone->fence->GetCompletedValue() < standalone->fenceValues[standalone->frameIndex])
		{
			standalone->fence->SetEventOnCompletion(standalone->fenceValues[standalone->frameIndex], standalone->fenceEvent);
			::WaitForSingleObjectEx(standalone->fenceEvent, INFINITE, FALSE);
		}

		standalone->frameIndex = standalone->swapChain->GetCurrentBackBufferIndex();
		standalone->fenceValues[standalone->frameIndex] = currentFenceValue + 1;
	}

	void WaitForGPU()
	{
		standalone->commandQueue->Signal(standalone->fence.Get(), standalone->fenceValues[standalone->frameIndex]);
		standalone->fence->SetEventOnCompletion(standalone->fenceValues[standalone->frameIndex], standalone->fenceEvent);
		::WaitForSingleObjectEx(standalone->fenceEvent, INFINITE, FALSE);
		++standalone->fenceValues[standalone->frameIndex];
	}

	void RenderDockSpace()
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("DOCKSPACE", NULL,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);
		ImGui::DockSpace(ImGui::GetID("DOCKSPACE"), {}, ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}
}

namespace ImGui
{
	void OnResize(int width, int height)
	{
		WaitPrevFrame();

		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			standalone->renderTargets[i].Reset();
			standalone->fenceValues[i] = standalone->fenceValues[standalone->frameIndex];
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		standalone->swapChain->GetDesc(&desc);
		standalone->swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		standalone->frameIndex = standalone->swapChain->GetCurrentBackBufferIndex();

		// 렌더 타겟 재생성
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ standalone->rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			standalone->swapChain->GetBuffer(i, IID_PPV_ARGS(&standalone->renderTargets[i]));
			standalone->d3dDevice->CreateRenderTargetView(standalone->renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(standalone->rtvDescriptorSize);
		}
		CreateRenderTargetView();
	}

	void Init(HWND _hWnd, ImGuiConfigFlags configFlags)
	{
		hWnd = _hWnd;

		standalone = Standalone{};
		CreateFactory();
		CreateDevice();
		CreateCommandQueue();
		CreateSwapChain();
		CreateRtvDescriptorHeap();
		CreateSrvDescriptorHeap(standalone->d3dDevice.Get());
		CreateRenderTargetView();
		CreateCommandList();
		CreateFence();
		ResetCommandList();
		ExecuteCommandList();
		WaitForGPU();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= configFlags;

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX12_Init(
			standalone->d3dDevice.Get(),
			FRAME_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			srvDescHeap.Get(),
			srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			srvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}

	void Init(HWND _hWnd, ID3D12Device* device, ID3D12GraphicsCommandList* _commandList, int num_frames_in_flight, DXGI_FORMAT rtv_format, ImGuiConfigFlags configFlags)
	{
		hWnd = _hWnd;
		commandList = _commandList;

		CreateSrvDescriptorHeap(device);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= configFlags;

		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX12_Init(
			device,
			num_frames_in_flight,
			rtv_format,
			srvDescHeap.Get(),
			srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			srvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}

	void BeginRender()
	{
		if (standalone)
		{
			ResetCommandList();
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(standalone->renderTargets[standalone->frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ standalone->rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(standalone->frameIndex), standalone->rtvDescriptorSize };
			commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

			constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
			commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		}

		commandList->SetDescriptorHeaps(1, srvDescHeap.GetAddressOf());
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (standalone)
			RenderDockSpace();
	}

	void EndRender()
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		if (standalone)
		{
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(standalone->renderTargets[standalone->frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
			commandList->Close();
			ID3D12CommandList* ppCommandList[]{ commandList.Get() };
			standalone->commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
			standalone->swapChain->Present(1, 0);
			WaitPrevFrame();
		}
	}

	void CleanUp()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool Button(std::wstring_view label, const ImVec2& size_arg)
	{
		auto utf8Label{ Util::wstou8s(label) };
		return Button(utf8Label.c_str(), size_arg);
	}
}
