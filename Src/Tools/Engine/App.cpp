#include "Stdafx.h"
#include "App.h"
#include "Clipboard.h"
#include "Delegates.h"
#include "Desktop.h"
#include "Explorer.h"
#include "FbxHandler.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "UIEditor.h"
#include "Viewport.h"

App::App() :
	m_isActive{ true }
{
	InitWindow();
	InitApp();
}

App::~App()
{
	Desktop::Destroy();
	Explorer::Destroy();
	Hierarchy::Destroy();
	Inspector::Destroy();
	Clipboard::Destroy();
	FbxHandler::Destroy();
	UIEditor::Destroy();
	Resource::CleanUp();
	Graphics::CleanUp();
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
	if (Graphics::ImGui::WndProcHandler(hWnd, message, wParam, lParam))
		return 1;

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
		if (wParam != SIZE_MINIMIZED)
		{
			int width{ GET_X_LPARAM(lParam) };
			int height{ GET_Y_LPARAM(lParam) };
			if (width == 0 && height == 0)
				assert(false);
			Delegates::OnWindowResized.Notify(width, height);
		}
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
	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);
}

void App::InitApp()
{
	// 모듈 초기화
	Graphics::Initialize(hWnd);
	Resource::Initialize(L"Engine", &Graphics::D2D::LoadSprite, &Graphics::D3D::LoadTexture, &Graphics::D3D::LoadModel);
	Delegates::OnWindowResized.Register(&Graphics::OnResize);

	// ImGui 초기화
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Engine/Imgui.ini";
	io.Fonts->AddFontFromFileTTF("Engine/NanumGothic.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	auto& style{ ImGui::GetStyle() };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
	
	// 싱글톤 생성
	Clipboard::Instantiate();
	Desktop::Instantiate();
	Explorer::Instantiate();
	FbxHandler::Instantiate();
	Hierarchy::Instantiate();
	Inspector::Instantiate();
	Viewport::Instantiate();

	// 타이머 초기화
	m_timer.Tick();
}

void App::Update()
{
	float deltaTime{ m_timer.Tick() };
	if (auto explorer{ Explorer::GetInstance() })
		explorer->Update(deltaTime);
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->Update(deltaTime);
	if (auto inspector{ Inspector::GetInstance() })
		inspector->Update(deltaTime);
	if (auto viewport{ Viewport::GetInstance() })
		viewport->Update(deltaTime);
}

void App::Render()
{
	Graphics::D3D::Begin();
	{
		Graphics::ImGui::Begin();
		{
			if (auto desktop{ Desktop::GetInstance() })
				desktop->Render();
			if (auto explorer{ Explorer::GetInstance() })
				explorer->Render();
			if (auto hierarchy{ Hierarchy::GetInstance() })
				hierarchy->Render();
			if (auto inspector{ Inspector::GetInstance() })
				inspector->Render();
			if (auto uiEditor{ UIEditor::GetInstance() })
				uiEditor->Render();
			if (auto viewport{ Viewport::GetInstance() })
				viewport->Render();
			ImGui::ShowDemoWindow();
		}
		Graphics::ImGui::End();
	}
	Graphics::D3D::End();
	Graphics::D2D::Begin();
	{
		if (auto uiEditor{ UIEditor::GetInstance() })
			uiEditor->Render2D();
	}
	Graphics::D2D::End();
	Graphics::D3D::Present();
}
