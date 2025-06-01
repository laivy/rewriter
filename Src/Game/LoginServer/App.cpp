#include "Stdafx.h"
#include "App.h"
#include "CenterServer.h"
#include "SocketManager.h"
#include "UserManager.h"

App::App()
{
	InitWindow();
	InitApp();
	m_timer.Tick();
}

App::~App()
{
	UserManager::Destroy(); // 접속 중인 유저 정보 저장
	CenterServer::Destroy(); // 센터 서버 연결 종료
	SocketManager::Destroy(); // 유저 접속 차단
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

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _IMGUI
	if (Graphics::ImGui::WndProcHandler(hWnd, message, wParam, lParam))
		return 1;
#endif

	App* app{ reinterpret_cast<App*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
	switch (message)
	{
	case WM_NCCREATE:
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
		return 1;
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
		wcex.hInstance,
		this
	);
	::SetWindowText(hWnd, WINDOW_TITLE_NAME);
	::ShowWindow(hWnd, SW_SHOWNORMAL);
	::UpdateWindow(hWnd);
}

void App::InitApp()
{
#ifdef _IMGUI
	Graphics::Initialize(hWnd);
	Resource::Initialize(L"Data");
	OnResize.Register(&Graphics::OnResize);

	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Data/imgui_login.ini";
	io.Fonts->AddFontFromFileTTF("Data/Galmuri11.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	auto& style{ ImGui::GetStyle() };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
#endif

	UserManager::Instantiate();
	SocketManager::Instantiate();
	CenterServer::Instantiate(); // 센터 서버 연결
}

void App::Update()
{
	auto deltaTime{ m_timer.Tick() };
	if (auto um{ UserManager::GetInstance() })
		um->Update(deltaTime);
}

void App::Render()
{
#if defined _IMGUI
	Graphics::D3D::Begin();
	{
		Graphics::ImGui::Begin();
		{
			if (auto sm{ SocketManager::GetInstance() })
				sm->Render();
		}
		Graphics::ImGui::End();
	}
	Graphics::D3D::End();
	Graphics::D3D::Present();
#endif
}
