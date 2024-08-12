#include "ImguiEx.h"
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"

#ifdef _IMGUI_STANDALONE
#include <dxgi1_6.h>
#include <wrl.h>
#include "External/DirectX/d3dx12.h"
using Microsoft::WRL::ComPtr;
#endif

namespace
{
#ifdef _IMGUI_STANDALONE
	constexpr auto FRAME_COUNT{ 3u };
	HWND hWnd;
	ComPtr<IDXGIFactory4> factory;
	ComPtr<ID3D12Device> d3dDevice;
	ComPtr<IDXGISwapChain3>	swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocators[FRAME_COUNT];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[FRAME_COUNT];
	ComPtr<ID3D12DescriptorHeap> srvDescHeap;
	ComPtr<ID3D12Fence> fence;
	D3D12_VIEWPORT viewport;
	UINT frameIndex;
	HANDLE fenceEvent;
	UINT64 fenceValues[FRAME_COUNT];
	UINT rtvDescriptorSize;
	UINT cbvSrvUavDescriptorIncrementSize;

	void CreateFactory()
	{
		::CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
	}

	void CreateDevice()
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		for (UINT i = 0; factory->EnumAdapters1(i, &hardwareAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			hardwareAdapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;
			::D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
			break;
		}
		if (!d3dDevice)
		{
			factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter));
			::D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
		}
		cbvSrvUavDescriptorIncrementSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
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
		factory->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		);

		swapChain1.As(&swapChain);
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	}

	void CreateRtvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = NULL;
		d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));
		rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	void CreateSrvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = NULL;
		d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescHeap));
	}

	void CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);

			d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i]));
		}
	}

	void CreateCommandList()
	{
		d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[frameIndex].Get(), nullptr, IID_PPV_ARGS(&commandList));
		commandList->Close();
	}

	void CreateFence()
	{
		d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		++fenceValues[frameIndex];
	}

	void ResetCommandList()
	{
		commandAllocators[frameIndex]->Reset();
		commandList->Reset(commandAllocators[frameIndex].Get(), nullptr);
	}

	void ExecuteCommandList()
	{
		commandList->Close();
		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void WaitPrevFrame()
	{
		const UINT64 currentFenceValue{ fenceValues[frameIndex] };
		commandQueue->Signal(fence.Get(), currentFenceValue);

		if (fence->GetCompletedValue() < fenceValues[frameIndex])
		{
			fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
			::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		}

		frameIndex = swapChain->GetCurrentBackBufferIndex();
		fenceValues[frameIndex] = currentFenceValue + 1;
	}

	void WaitForGPU()
	{
		commandQueue->Signal(fence.Get(), fenceValues[frameIndex]);
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		++fenceValues[frameIndex];
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
#else
	HWND hWnd;
	ID3D12DescriptorHeap* srvDescHeap;
	ID3D12GraphicsCommandList* commandList;

	void CreateSrvDescriptorHeap(ID3D12Device* device)
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = NULL;
		device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescHeap));
	}
#endif
}

namespace ImGui
{
#ifdef _IMGUI_STANDALONE
	void Init(HWND _hWnd, ImGuiConfigFlags configFlags)
	{
		hWnd = _hWnd;
		CreateFactory();
		CreateDevice();
		CreateCommandQueue();
		CreateSwapChain();
		CreateRtvDescriptorHeap();
		CreateSrvDescriptorHeap();
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
			d3dDevice.Get(),
			FRAME_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			srvDescHeap.Get(),
			srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			srvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}

	void OnResize(int width, int height)
	{
		WaitPrevFrame();

		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			renderTargets[i].Reset();
			fenceValues[i] = fenceValues[frameIndex];
		}

		DXGI_SWAP_CHAIN_DESC desc{};
		swapChain->GetDesc(&desc);
		swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		// 렌더 타겟 재생성
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart() };
		for (unsigned int i = 0; i < FRAME_COUNT; ++i)
		{
			swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
			d3dDevice->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(rtvDescriptorSize);
		}
		CreateRenderTargetView();
	}
#else
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
			srvDescHeap,
			srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			srvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}
#endif

	void BeginRender()
	{
#ifdef _IMGUI_STANDALONE
		ResetCommandList();
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(frameIndex), rtvDescriptorSize };
		commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		commandList->SetDescriptorHeaps(1, srvDescHeap.GetAddressOf());
#else
		commandList->SetDescriptorHeaps(1, &srvDescHeap);
#endif
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#ifdef _IMGUI_STANDALONE
		RenderDockSpace();
#endif
	}

	void EndRender()
	{
		ImGui::Render();
#ifdef _IMGUI_STANDALONE
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
#else
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#ifdef _IMGUI_STANDALONE
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
		commandList->Close();
		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

		swapChain->Present(1, 0);
		WaitPrevFrame();
#endif
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
