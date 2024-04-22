#include "Stdafx.h"
#include "App.h"
#include "EventManager.h"
#include "LoginServer.h"
#include "ObjectManager.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Wnd.h"
#include "WndManager.h"
#include "Common/Timer.h"

App::App() :
	m_isActive{ true },
	m_hInstance{ NULL },
	m_hWnd{ NULL },
	m_size{ 1920, 1080 },
	m_timer{ new Timer }
{
}

void App::OnCreate()
{
	InitWindow();
	Renderer3D::Init();

	EventManager::Instantiate();
	ObjectManager::Instantiate();
	WndManager::Instantiate();
	SceneManager::Instantiate();

	if (auto loginServer{ LoginServer::Instantiate() })
		loginServer->Connect();

	m_timer->Tick();
}

void App::Run()
{
	MSG msg{};
	while (m_isActive)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

HWND App::GetHwnd() const
{
	return m_hWnd;
}

INT2 App::GetWindowSize() const
{
	return m_size;
}

INT2 App::GetCursorPosition() const
{
	POINT mouse;
	::GetCursorPos(&mouse);
	::ScreenToClient(m_hWnd, &mouse);
	return { mouse.x, mouse.y };
}

LRESULT CALLBACK App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	App* app{ reinterpret_cast<App*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
	switch (message)
	{
	case WM_NCCREATE:
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
		return 1;
	}
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_IME_COMPOSITION:
		App::OnKeyboardEvent.Notify(message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		App::OnMouseEvent.Notify(message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_SIZE:
		App::OnResize.Notify(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_DESTROY:
		app->OnDestroy();
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void App::OnDestroy()
{
	Renderer3D::CleanUp();

	ObjectManager::Destroy();
	WndManager::Destroy();
	EventManager::Destroy();
	SceneManager::Destroy();
}

HRESULT App::InitWindow()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"CLIENT";
	::RegisterClassEx(&wcex);
	
	RECT rect{ 0, 0, m_size.x, m_size.y };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hWnd = ::CreateWindow(
		L"CLIENT",
		L"CLIENT",
		WS_OVERLAPPEDWINDOW, /*WS_OVERLAPPED | WS_SYSMENU | WS_BORDER,*/
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);
	::SetWindowText(m_hWnd, TEXT("Rewriter"));

	HRESULT hr{ m_hWnd ? S_OK : E_FAIL };
	if (SUCCEEDED(hr))
	{
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);
		::UpdateWindow(m_hWnd);
	}
	return hr;
}

void App::Update()
{
	float deltaTime{ m_timer->Tick() };
	if (auto sm{ SceneManager::GetInstance() })
		sm->Update(deltaTime);
}

void App::Render()
{
	Renderer3D::RenderStart();
	{
		//if (SceneManager::IsInstanced())
		//	SceneManager::GetInstance()->Render(m_commandList);
	}
	Renderer3D::RenderEnd();

	Renderer2D::RenderStart();
	{
		if (auto sm{ SceneManager::GetInstance() })
			sm->Render2D();
	}
	Renderer2D::RenderEnd();

	Renderer3D::Present();
}