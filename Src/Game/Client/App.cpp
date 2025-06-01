#include "Stdafx.h"
#include "App.h"
#include "LoginServer.h"
#include "SceneManager.h"
#include "SocketManager.h"
#include "Window.h"
#include "WindowManager.h"

App::App()
{
	InitWindow();
	InitApp();
	m_timer.Tick();
}

App::~App()
{
	SceneManager::Destroy();
	SocketManager::Destroy();
	Resource::CleanUp();
	Graphics::CleanUp();
}

void App::Run()
{
	MSG msg{};
	while (true)
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
#ifdef _IMGUI
	if (Graphics::ImGui::WndProcHandler(hWnd, message, wParam, lParam))
		return 1;
#endif // _IMGUI

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
	{
		App::OnKeyboardEvent.Notify(message, wParam, lParam);
		break;
	}
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	{
		App::OnMouseEvent.Notify(message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	}
	case WM_SIZE:
	{
		App::OnResize.Notify(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	}
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		break;
	}
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
	wcex.lpszClassName = WINDOW_TITLE_NAME;
	::RegisterClassEx(&wcex);
	
	RECT rect{ 0, 0, size.x, size.y };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = ::CreateWindow(
		wcex.lpszClassName,
		WINDOW_TITLE_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);
	::SetWindowText(hWnd, WINDOW_TITLE_NAME);
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::UpdateWindow(hWnd);
}

void App::InitApp()
{
	// 모듈 초기화
	Graphics::Initialize(hWnd);
	Resource::Initialize(L"Data", &Graphics::D2D::LoadSprite, &Graphics::D3D::LoadTexture, &Graphics::D3D::LoadModel);
	OnResize.Register(&Graphics::OnResize);

#ifdef _IMGUI
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Data/imgui_client.ini";
	io.Fonts->AddFontFromFileTTF("Data/Galmuri11.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	auto& style{ ImGui::GetStyle() };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
#endif

	// 싱글톤 생성
	SocketManager::Instantiate();
	LoginServer::Instantiate();
	SceneManager::Instantiate();
}

void App::Update()
{
	float deltaTime{ m_timer.Tick() };
	if (auto wm{ WindowManager::GetInstance() })
		wm->Update(deltaTime);
	if (auto sm{ SceneManager::GetInstance() })
		sm->Update(deltaTime);
}

void App::Render()
{
	Graphics::D3D::Begin();
	{
		if (auto sm{ SceneManager::GetInstance() })
			sm->Render3D();
#ifdef _IMGUI
		Graphics::ImGui::Begin();
		{
			ImGui::ShowDemoWindow();
		}
		Graphics::ImGui::End();
#endif // _IMGUI
	}
	Graphics::D3D::End();
	Graphics::D2D::Begin();
	{
		if (auto sm{ SceneManager::GetInstance() })
			sm->Render2D();
	}
	Graphics::D2D::End();
	Graphics::D3D::Present();
}
