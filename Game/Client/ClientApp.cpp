#include "Stdafx.h"
#include "ClientApp.h"
#include "EventManager.h"
#include "LoginServer.h"
#include "ObjectManager.h"
#include "Renderer2D.h"
#include "SceneManager.h"
#include "Wnd.h"
#include "WndManager.h"
#include "Common/Timer.h"

ClientApp::ClientApp() :
	m_isActive{ true },
	m_hInstance{ NULL },
	m_hWnd{ NULL },
	m_size{ 1920, 1080 },
	m_viewport{},
	m_scissorRect{},
	m_frameIndex{ 0 },
	m_fenceEvent{ NULL },
	m_fenceValues{ 0, },
	m_rtvDescriptorSize{ 0 },
	m_timer{ new Timer }
{
}

bool ClientApp::OnCreate()
{
#ifndef _NO_SERVER
	// 로그인 서버 연결에 실패하면 바로 클라이언트 종료
	if (auto lgnSvr{ LoginServer::Instantiate() }; !lgnSvr->Connect())
	{
		m_isActive = false;
		return false;
	}
#endif

	InitWindow();
	InitDirectX();
	ResetCommandList();

	EventManager::Instantiate();
	ObjectManager::Instantiate();
	WndManager::Instantiate();
	SceneManager::Instantiate();

	ExecuteCommandList();
	WaitForGPU();

	m_timer->Tick();
	return true;
}

void ClientApp::Run()
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

HWND ClientApp::GetHwnd() const
{
	return m_hWnd;
}

INT2 ClientApp::GetWindowSize() const
{
	return m_size;
}

INT2 ClientApp::GetCursorPosition() const
{
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(m_hWnd, &mouse);
	return { mouse.x, mouse.y };
}

ComPtr<ID3D12Device> ClientApp::GetD3DDevice() const
{
	return m_d3dDevice;
}

ComPtr<ID3D12GraphicsCommandList> ClientApp::GetCommandList() const
{
	return m_commandList;
}

ComPtr<ID3D12RootSignature> ClientApp::GetRootSignature() const
{
	return m_rootSignature;
}

ComPtr<IDWriteFactory5> ClientApp::GetDwriteFactory() const
{
	return m_dwriteFactory;
}

LRESULT CALLBACK ClientApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ClientApp* app{ reinterpret_cast<ClientApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
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
	case WM_MOUSEMOVE:
		app->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		app->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONDOWN:
		app->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONUP:
		app->OnRButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		app->OnRButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_CHAR:
	case WM_IME_COMPOSITION:
	case WM_KEYUP:
	case WM_KEYDOWN:
		app->OnKeyboardEvent(message, wParam, lParam);
		break;
	case WM_DESTROY:
		app->OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ClientApp::OnDestroy()
{
	WaitForGPU();
	CloseHandle(m_fenceEvent);

	ObjectManager::Destroy();
	WndManager::Destroy();
	EventManager::Destroy();
	SceneManager::Destroy();

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL);
	}
#endif
}

void ClientApp::OnResize(int width, int height)
{
	m_viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	m_scissorRect = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnResize(width, height);
}

void ClientApp::OnMouseMove(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnMouseMove(x, y);
}

void ClientApp::OnLButtonUp(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnLButtonUp(x, y);
}

void ClientApp::OnLButtonDown(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnLButtonDown(x, y);
}

void ClientApp::OnRButtonUp(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnRButtonUp(x, y);
}

void ClientApp::OnRButtonDown(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnRButtonDown(x, y);
}

void ClientApp::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnKeyboardEvent(message, wParam, lParam);
}

HRESULT ClientApp::InitWindow()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"GAME";
	RegisterClassEx(&wcex);
	
	RECT rect{ 0, 0, m_size.x, m_size.y };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hWnd = CreateWindow(
		L"GAME",
		L"GAME",
		WS_OVERLAPPED | WS_SYSMENU | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);
	SetWindowText(m_hWnd, TEXT("Rewriter"));

	HRESULT hr{ m_hWnd ? S_OK : E_FAIL };
	if (SUCCEEDED(hr))
	{
		ShowWindow(m_hWnd, SW_SHOWNORMAL);
		UpdateWindow(m_hWnd);
	}
	return hr;
}

HRESULT ClientApp::InitDirectX()
{
	CreateFactory();
	CreateDevice();
	CreateCommandQueue();

	// D3D11on12, D2D
	CreateD3D11On12Device();
	CreateD2DFactory();
	CreateD2DDevice();

	CreateSwapChain();
	CreateRtvDsvDescriptorHeap();
	CreateRenderTargetView();
	CreateDepthStencilView();
	CreateRootSignature();
	CreateCommandList();
	CreateFence();

	return S_OK;
}

void ClientApp::CreateFactory()
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

	DX::ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));
}

void ClientApp::CreateDevice()
{
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
		DX::ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice)));
	}
	g_cbvSrvUavDescriptorIncrementSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ClientApp::CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc{};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	DX::ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
}

void ClientApp::CreateD3D11On12Device()
{
	ComPtr<ID3D11Device> d3d11Device;
	DX::ThrowIfFailed(D3D11On12CreateDevice(
		m_d3dDevice.Get(),
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		reinterpret_cast<IUnknown**>(m_commandQueue.GetAddressOf()),
		1,
		0,
		&d3d11Device,
		&m_d3d11DeviceContext,
		nullptr
	));

	DX::ThrowIfFailed(d3d11Device.As(&m_d3d11On12Device));
}

void ClientApp::CreateD2DFactory()
{
	DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf()));
}

void ClientApp::CreateD2DDevice()
{
	ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(m_d3d11On12Device.As(&dxgiDevice));
	DX::ThrowIfFailed(m_d2dFactory->CreateDevice(dxgiDevice.Get(), &m_d2dDevice));
	DX::ThrowIfFailed(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &Renderer2D::g_ctx));
	DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &m_dwriteFactory));
}

void ClientApp::CreateSwapChain()
{
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
	DX::ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),
		m_hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	DX::ThrowIfFailed(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// ALT + ENTER 금지
	DX::ThrowIfFailed(m_factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void ClientApp::CreateRtvDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.NumDescriptors = FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = NULL;
	DX::ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
}

void ClientApp::CreateRenderTargetView()
{
	// D2D 렌더타겟 속성
	UINT dpi{ GetDpiForWindow(m_hWnd) };
	D2D1_BITMAP_PROPERTIES1 bitmapProperties{ D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		static_cast<float>(dpi),
		static_cast<float>(dpi)
	) };

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < FRAME_COUNT; ++i)
	{
		// DX12
		DX::ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
		m_d3dDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(m_rtvDescriptorSize);

		// D3D11on12
		D3D11_RESOURCE_FLAGS d3d11Flags{ D3D11_BIND_RENDER_TARGET };
		DX::ThrowIfFailed(m_d3d11On12Device->CreateWrappedResource(
			m_renderTargets[i].Get(),
			&d3d11Flags,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(&m_wrappedBackBuffers[i])
		));

		// D2D
		ComPtr<IDXGISurface> surface;
		DX::ThrowIfFailed(m_wrappedBackBuffers[i].As(&surface));
		DX::ThrowIfFailed(Renderer2D::g_ctx->CreateBitmapFromDxgiSurface(
			surface.Get(),
			&bitmapProperties,
			&m_d2dRenderTargets[i]
		));

		// CommandAllocator
		DX::ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
	}
}

void ClientApp::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = m_size.x;
	desc.Height = m_size.y;
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

	DX::ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&m_depthStencil)
	));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilViewDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void ClientApp::CreateRootSignature()
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
	DX::ThrowIfFailed(m_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void ClientApp::CreateCommandList()
{
	DX::ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
	DX::ThrowIfFailed(m_commandList->Close());
}

void ClientApp::CreateFence()
{
	DX::ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	++m_fenceValues[m_frameIndex];
}

void ClientApp::Update()
{
	// Update에서도 커맨드리스트에 명령을 추가하는 경우가 있음
	ResetCommandList();

	m_timer->Tick();

	float deltaTime{ m_timer->GetDeltaTime() };
	if (auto sm{ SceneManager::GetInstance() })
		sm->Update(deltaTime);
}

void ClientApp::Render()
{
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(m_frameIndex), m_rtvDescriptorSize };
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ m_dsvHeap->GetCPUDescriptorHandleForHeapStart() };
	m_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
	m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	//if (auto srvDescHeap{ ResourceManager::GetInstance()->GetSrvDescriptorHeap() })
	//	m_commandList->SetDescriptorHeaps(1, srvDescHeap);

	//if (SceneManager::IsInstanced())
	//	SceneManager::GetInstance()->Render(m_commandList);

	ExecuteCommandList();

	// ------------

	m_d3d11On12Device->AcquireWrappedResources(m_wrappedBackBuffers[m_frameIndex].GetAddressOf(), 1);
	Renderer2D::g_ctx->SetTarget(m_d2dRenderTargets[m_frameIndex].Get());
	Renderer2D::g_ctx->BeginDraw();

	if (auto sm{ SceneManager::GetInstance() })
		sm->Render2D();

	DX::ThrowIfFailed(Renderer2D::g_ctx->EndDraw());
	m_d3d11On12Device->ReleaseWrappedResources(m_wrappedBackBuffers[m_frameIndex].GetAddressOf(), 1);
	m_d3d11DeviceContext->Flush();

	// ------------

	DX::ThrowIfFailed(m_swapChain->Present(1, 0));
	WaitPrevFrame();
}

void ClientApp::ResetCommandList() const
{
	DX::ThrowIfFailed(m_commandAllocators[m_frameIndex]->Reset());
	DX::ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr));
}

void ClientApp::ExecuteCommandList() const
{
	DX::ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* ppCommandList[]{ m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
}

void ClientApp::WaitPrevFrame()
{
	const UINT64 currentFenceValue{ m_fenceValues[m_frameIndex] };
	DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

	if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
	{
		DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

void ClientApp::WaitForGPU()
{
	DX::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));
	DX::ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
	WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	++m_fenceValues[m_frameIndex];
}
