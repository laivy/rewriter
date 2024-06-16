#include "Stdafx.h"
#include "App.h"
#include "Explorer.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Common/ImguiEx.h"
#include "Common/Timer.h"

App::App() :
	m_isActive{ true },
	m_timer{ new Timer }
{
	InitWindow();
	InitImGui();
	InitApp();
	m_timer->Tick();
}

App::~App()
{
	Explorer::Destroy();
	Hierarchy::Destroy();
	Inspector::Destroy();
	ImGui::CleanUp();	
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
	{
		OnResize->Notify(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_TITLE_NAME;
	::RegisterClassEx(&wcex);

	// 화면 최대 크기로 윈도우 생성
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
	::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
	::UpdateWindow(hWnd);
}

void App::InitImGui()
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
	Explorer::Instantiate();
	Hierarchy::Instantiate();
	Inspector::Instantiate();
}

void App::Update()
{
	float deltaTime{ m_timer->Tick() };
	if (auto explorer{ Explorer::GetInstance() })
		explorer->Update(deltaTime);
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->Update(deltaTime);
	if (auto inspector{ Inspector::GetInstance() })
		inspector->Update(deltaTime);
}

void App::Render()
{
	ImGui::RenderBegin();
	{
		RenderImGuiMainDockSpace();
		if (auto explorer{ Explorer::GetInstance() })
			explorer->Render();
		if (auto hierarchy{ Hierarchy::GetInstance() })
			hierarchy->Render();
		if (auto inspector{ Inspector::GetInstance() })
			inspector->Render();
		ImGui::ShowDemoWindow();
	}
	ImGui::RenderEnd();
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

	ImGui::PushID("DESKTOP");
	ImGui::Begin("DESKTOP", NULL, windowFlag);
	ImGui::DockSpace(ImGui::GetID("DESKTOP"), {}, ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
	ImGui::PopID();
}