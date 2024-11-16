#include "Stdafx.h"
#include "App.h"
#include "SocketManager.h"
#ifdef _IMGUI
#include "Common/ImguiEx.h"
#endif

App::App()
{
	InitWindow();
	InitImgui();
	InitApp();
	m_timer.Tick();
}

App::~App()
{
	SocketManager::Destroy();
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
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
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

void App::InitImgui()
{
#ifdef _IMGUI
	ImGui::Init(hWnd, ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable);
	OnResize.Register(&ImGui::OnResize);

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Data/imgui_center.ini";
	io.Fonts->AddFontFromFileTTF("Data/Galmuri11.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	auto& style{ ImGui::GetStyle() };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
#endif
}

void App::InitApp()
{
	Database::Initialize(Resource::Get(L"Server.dat/CenterServer/Info/Database"));

	// Database 샘플
	{
		//auto result{ Database::Select{ Database::Type::Game }
		//	.Statement(L"SELECT * FROM [dbo].[account] WHERE [register_date] > ?")
		//	.Param(1, Time{ 2024, 11, 1, 0, 0, 0 })
		//	.Execute()
		//};

		//int64_t id{};
		//std::wstring name(16, L'\0');
		//std::wstring password(16, L'\0');
		//Time registerDate{};
		//result.Bind(1, &id)
		//	.Bind(2, &name)
		//	.Bind(3, &password)
		//	.Bind(4, &registerDate);
		//while (result.Fetch())
		//{
		//	::OutputDebugString(L"FETCH\n");
		//}

		int32_t ret{ -1 };
		Database::StoredProcedure{ Database::Type::Game }
			.Statement(L"{ CALL [get_account_id] (?, ?) }")
			.Out(1, &ret)
			.In(2, L"laivy3")
			.Execute();

		int i = 0;
	}
	
	SocketManager::Instantiate();
}

void App::Update()
{
}

void App::Render()
{
#ifdef _IMGUI
	ImGui::BeginRender();
	{
		if (auto sm{ SocketManager::GetInstance() })
			sm->Render();
	}
	ImGui::EndRender();
#endif
}
