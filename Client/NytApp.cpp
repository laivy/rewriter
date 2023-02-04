#include "Stdafx.h"
#include "NytApp.h"
#include "NytLoader.h"
#include "NytImage.h"
#include "NytProperty.h"
#include "BrushPool.h"
#include "Timer.h"
#include "FontPool.h"
#include "KeyWorkerThread.h"
#include "MouseWorkerThread.h"
#include "WndManager.h"
#include "Wnd.h"
#include "SceneManager.h"

NytApp::NytApp() : m_hwnd{ NULL }, m_size{ 1920, 1080 }, m_timer{ new Timer }
{
	HRESULT hr{ E_FAIL };
	hr = InitWnd();
	hr = InitD2D();
	assert(SUCCEEDED(hr));
}

void NytApp::OnCreate()
{
	FontPool::Instantiate();
	BrushPool::Instantiate(m_renderTarget);
	NytLoader::Instantiate();

	WndManager::Instantiate();
	SceneManager::Instantiate();

	KeyboardWorkerThread::Instantiate();
	MouseWorkerThread::Instantiate();

	m_timer->Tick();
}

void NytApp::OnDestroy()
{
	if (SceneManager::IsInstanced())
		SceneManager::GetInstance()->OnDestroy();
}

void NytApp::Run()
{
	MSG msg{};
	while (TRUE)
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

HWND NytApp::GetHwnd() const
{
	return m_hwnd;
}

INT2 NytApp::GetSize() const
{
	return m_size;
}

ComPtr<IDWriteFactory5> NytApp::GetDwriteFactory() const
{
	return m_dwriteFactory;
}

ComPtr<ID2D1HwndRenderTarget> NytApp::GetRenderTarget() const
{
	return m_renderTarget;
}

HRESULT NytApp::InitD2D()
{
	HRESULT hr{ E_FAIL };
	hr = CoInitialize(NULL);
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(m_dwriteFactory),
		reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
	);

	RECT rc;
	GetClientRect(m_hwnd, &rc);

	INT2 size{ rc.right - rc.left, rc.bottom - rc.top };
	hr = m_d2dFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_hwnd, size),
		&m_renderTarget
	);

	return hr;
}

HRESULT NytApp::InitWnd()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D2DApp";
	RegisterClassEx(&wcex);

	m_hwnd = CreateWindow(
		L"D2DApp",
		L"App",
		WS_OVERLAPPED | WS_SYSMENU | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_size.x,
		m_size.y,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);

	HRESULT hr{ m_hwnd ? S_OK : E_FAIL };
	if (SUCCEEDED(hr))
	{
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
	}
	return hr;
}

LRESULT CALLBACK NytApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NytApp* app{};
	if (message == WM_NCCREATE)
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		app = static_cast<NytApp*>(pcs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
		return 1;
	}

	app = reinterpret_cast<NytApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if (SceneManager::IsInstanced())
			SceneManager::GetInstance()->OnMouseEvent(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (SceneManager::IsInstanced())
			SceneManager::GetInstance()->OnKeyboardEvent(hWnd, message, wParam, lParam);
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

void NytApp::Update()
{
	m_timer->Tick();
	FLOAT deltaTime{ m_timer->GetDeltaTime() };

	if (SceneManager::IsInstanced())
		SceneManager::GetInstance()->Update(deltaTime);
}

void NytApp::Render() const
{
	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF{ D2D1::ColorF::White });

	if (SceneManager::IsInstanced())
		SceneManager::GetInstance()->Render(m_renderTarget);

	m_renderTarget->EndDraw();
}