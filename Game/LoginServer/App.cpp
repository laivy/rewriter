#include "Stdafx.h"
#include "Acceptor.h"
#include "App.h"
#include "DBAccessor.h"
#include "SocketManager.h"
#include "UserManager.h"
#include "Common/ImguiEx.h"

App::App()
{
	InitWindow();
	InitImgui();
	InitApp();
}

App::~App()
{
	Acceptor::Destroy(); // 1. 유저 접속 차단
	UserManager::Destroy(); // 2. 접속 중인 유저 정보 저장
	DBAccessor::Destroy(); // 3. DB 연결 해제
	ImGui::CleanUp();
	::WSACleanup();
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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

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

void App::InitImgui()
{
	ImGui::Init(hWnd);
	OnResize->Register(&ImGui::OnResize);

	ImGuiIO& io{ ImGui::GetIO() };
	io.IniFilename = "Data/imgui.ini";
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.Fonts->AddFontFromFileTTF("Data/NEXON Lv2 Gothic.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;
	ImGui::GetStyle().DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
}

void App::InitApp()
{
	WSADATA wsaData{};
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		assert(false && "FAIL WSAStartup");

	DBAccessor::Instantiate(); // DB 연결
	UserManager::Instantiate();
	SocketManager::Instantiate(); 
	Acceptor::Instantiate(); // 유저 접속 허용
}

void App::Update()
{
	if (auto um{ UserManager::GetInstance() })
		um->Update();
}

void App::Render()
{
	ImGui::RenderBegin();
	{
		RenderBackgroundWindow();
		if (auto dbThread{ DBAccessor::GetInstance() })
			dbThread->Render();
		if (auto acceptThread{ Acceptor::GetInstance() })
			acceptThread->Render();
	}
	ImGui::RenderEnd();
}

void App::RenderBackgroundWindow()
{
	RECT rect{};
	::GetClientRect(hWnd, &rect);

	ImGui::Begin("Background", NULL,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking);
	ImGui::SetWindowSize({ static_cast<float>(rect.right), static_cast<float>(rect.bottom) });
	ImGui::SetWindowPos({ 0.0f, 0.0f });
	ImGui::DockSpace(ImGui::GetID("Background"), {}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}