#include "Stdafx.h"
#include "App.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Server.h"
#include "Window.h"
#include "WindowManager.h"
#include "Common/Timer.h"

App::App() :
	m_isActive{ true },
	m_timer{ new Timer }
{
	InitWindow();
	Renderer::Init();
	InitApp();
	m_timer->Tick();
}

App::~App()
{
	Renderer::CleanUp();
	SceneManager::Destroy();
	WindowManager::Destroy();
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
		App::OnKeyboardEvent->Notify(message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		App::OnMouseEvent->Notify(message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_SIZE:
		App::OnResize->Notify(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
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
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"CLIENT";
	::RegisterClassEx(&wcex);
	
	RECT rect{ 0, 0, size.x, size.y };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = ::CreateWindow(
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
	::SetWindowText(hWnd, TEXT("Rewriter"));
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::UpdateWindow(hWnd);
}

void App::InitApp()
{
	SceneManager::Instantiate();
	WindowManager::Instantiate();

	Connect<LoginServer>();
}

void App::Update()
{
	float deltaTime{ m_timer->Tick() };
	if (auto wm{ WindowManager::GetInstance() })
		wm->Update(deltaTime);
	if (auto sm{ SceneManager::GetInstance() })
		sm->Update(deltaTime);
}

void App::Render()
{
	Renderer3D::Begin();
	{
		if (auto sm{ SceneManager::GetInstance() })
			sm->Render3D();
	}
	Renderer3D::End();
	Renderer2D::Begin();
	{
		if (auto sm{ SceneManager::GetInstance() })
			sm->Render2D();
	}
	Renderer2D::End();
	Renderer::Present();
}