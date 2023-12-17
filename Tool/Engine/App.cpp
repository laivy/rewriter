#include "Stdafx.h"
#include "App.h"
#include "ExPlorer.h"
#include "Hierarchy.h"
#include "Inspector.h"

App::App(HINSTANCE hInstance) :
	m_isActive{ false },
	m_hInstance{ hInstance },
	m_hWnd{ NULL },
	m_size{ 1920, 1080 }
{
	InitWindow();
	InitDirectX();
	InitImGui();
	m_isActive = true;
}

App::~App()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void App::Run()
{
	MSG msg{};
	while (m_isActive)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	App* app{ reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
	switch (message)
	{
	case WM_NCCREATE:
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
		return 1;
	}
	case WM_SIZE:
		app->OnResize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void App::InitWindow()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TITLE_NAME;
	RegisterClassEx(&wcex);

	// 화면 최대 크기로 윈도우 생성
	RECT rect{ 0, 0, m_size.first, m_size.second };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hWnd = CreateWindow(
		wcex.lpszClassName,
		TITLE_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		wcex.hInstance,
		this
	);
	SetWindowText(m_hWnd, TITLE_NAME);

	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(m_hWnd);
}

void App::InitDirectX()
{
	// 팩토리, 디바이스 생성
	CreateDXGIFactory2(NULL, IID_PPV_ARGS(&m_factory));
	ComPtr<IDXGIAdapter1> hardwareAdapter;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(i, &hardwareAdapter); ++i)
	{
		DXGI_ADAPTER_DESC1 adapterDesc{};
		hardwareAdapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)))) break;
	}
	if (!m_d3dDevice)
	{
		m_factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter));
		D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
	}
	m_cbvSrvUavDescriptorIncrementSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 커맨드 큐, 할당자, 리스트 생성
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	for (int i{ 0 }; i < FRAME_COUNT; ++i)
		m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i]));

	m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), nullptr, IID_PPV_ARGS(&m_commandList));
	m_commandList->Close();

	// 스왑체인 생성
	RECT rect{};
	GetClientRect(m_hWnd, &rect);

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.BufferCount = FRAME_COUNT;
	swapChainDesc.Width = rect.right - rect.left;
	swapChainDesc.Height = rect.bottom - rect.top;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	m_factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	swapChain.As(&m_swapChain);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	// 서술자 힙 생성
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;
	m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NodeMask = NULL;
	m_d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvDescHeap));

	// 렌더타겟 생성
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FRAME_COUNT; ++i)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		m_d3dDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(m_rtvDescriptorSize);
	}

	// 펜스 생성
	m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	++m_fenceValues[m_frameIndex];
}

void App::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io{ ImGui::GetIO() };
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
	ImGui::GetStyle().DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX12_Init(
		m_d3dDevice.Get(),
		FRAME_COUNT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		m_srvDescHeap.Get(),
		m_srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_srvDescHeap->GetGPUDescriptorHandleForHeapStart()
	);

	// 윈도우 싱글턴 생성
	Explorer::Instantiate();
	Hierarchy::Instantiate();
	Inspector::Instantiate();
}

void App::OnResize(int width, int height)
{
	if (!m_isActive)
		return;

	WaitPrevFrame();

	for (int i = 0; i < FRAME_COUNT; ++i)
	{
		m_renderTargets[i].Reset();
		m_fenceValues[i] = m_fenceValues[m_frameIndex];
	}

	DXGI_SWAP_CHAIN_DESC desc{};
	m_swapChain->GetDesc(&desc);
	m_swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	
	// 렌더타겟 생성
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FRAME_COUNT; ++i)
	{
		m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
		m_d3dDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(m_rtvDescriptorSize);
	}
}

void App::Update()
{
}

void App::Render()
{
	m_commandAllocators[m_frameIndex]->Reset();
	m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(m_frameIndex), m_rtvDescriptorSize };
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	constexpr FLOAT clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	m_commandList->SetDescriptorHeaps(1, m_srvDescHeap.GetAddressOf());

	RenderImGui();

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	m_commandList->Close();
	ID3D12CommandList* ppCommandList[]{ m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);

	m_swapChain->Present(1, 0);
	WaitPrevFrame();
}

void App::WaitPrevFrame()
{
	const UINT64 currentFenceValue{ m_fenceValues[m_frameIndex] };
	m_commandQueue->Signal(m_fence.Get(), currentFenceValue);

	if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
	{
		m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent);
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

void App::RenderImGui()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	RenderImGuiMainDockSpace();
	Explorer::GetInstance()->Render();
	RenderImGuiConsole();
	Hierarchy::GetInstance()->Render();
	Inspector::GetInstance()->Render();
	ImGui::ShowDemoWindow();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void App::RenderImGuiMainDockSpace()
{
	ImGuiWindowFlags windowFlag{ ImGuiWindowFlags_NoDocking };

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	windowFlag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	windowFlag |= ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiDockNodeFlags dockSpaceFlag{ ImGuiDockNodeFlags_PassthruCentralNode };
	ImGui::Begin("MainDockSpace", NULL, windowFlag);
	ImGui::DockSpace(ImGui::GetID("MainDockSpace"), ImVec2{}, dockSpaceFlag);
	ImGui::End();
}

void App::RenderImGuiConsole()
{
	ImGui::Begin("Console");
	ImGui::End();
}
