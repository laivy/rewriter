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
	m_viewport{},
	m_scissorRect{},
	m_frameIndex{ 0 },
	m_fenceEvent{ NULL },
	m_fenceValues{ 0, },
	m_rtvDescriptorSize{ 0 },
	m_timer{ new Timer }
{
}

bool App::OnCreate()
{
	InitWindow();
	Renderer3D::Init();
	Renderer2D::Init();

	EventManager::Instantiate();
	ObjectManager::Instantiate();
	WndManager::Instantiate();
	SceneManager::Instantiate();

	if (auto loginServer{ LoginServer::Instantiate() })
	{
#ifdef _DEBUG
		//bool retry{ !loginServer->Connect() };
		//while (retry)
		//{
		//	retry = !loginServer->Connect();
		//}
#else
		if (!loginServer->Connect())
		{
			m_isActive = false;
			return false;
		}
#endif
	}

	m_timer->Tick();
	return true;
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
	GetCursorPos(&mouse);
	ScreenToClient(m_hWnd, &mouse);
	return { mouse.x, mouse.y };
}

LRESULT CALLBACK App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

void App::OnDestroy()
{
	Renderer3D::CleanUp();

	ObjectManager::Destroy();
	WndManager::Destroy();
	EventManager::Destroy();
	SceneManager::Destroy();
}

void App::OnResize(int width, int height)
{
	m_viewport = D3D12_VIEWPORT{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	m_scissorRect = D3D12_RECT{ 0, 0, static_cast<long>(width), static_cast<long>(height) };
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnResize(width, height);
}

void App::OnMouseMove(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnMouseMove(x, y);
}

void App::OnLButtonUp(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnLButtonUp(x, y);
}

void App::OnLButtonDown(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnLButtonDown(x, y);
}

void App::OnRButtonUp(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnRButtonUp(x, y);
}

void App::OnRButtonDown(int x, int y)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnRButtonDown(x, y);
}

void App::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (auto sm{ SceneManager::GetInstance() })
		sm->OnKeyboardEvent(message, wParam, lParam);
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