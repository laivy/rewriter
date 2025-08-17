#include "Stdafx.h"
#include "App.h"
#include "Delegates.h"

App::App() :
	m_hWnd{ NULL },
	m_windowSize{ 1920, 1080 }
{
	InitWindow();
	InitApp();
}

App::~App()
{
	Resource::Uninitialize();
	Graphics::Uninitialize();
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

Int2 App::GetCursorPosition() const
{
	POINT mouse{};
	if (!::GetCursorPos(&mouse))
		return Int2{};
	if (!::ScreenToClient(m_hWnd, &mouse))
		return Int2{};
	return Int2{ static_cast<int32_t>(mouse.x), static_cast<int32_t>(mouse.y) };
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
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_TITLE_NAME;
	::RegisterClassEx(&wcex);

	// 화면 최대 크기로 윈도우 생성
	RECT rect{ 0, 0, m_windowSize.x, m_windowSize.y };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hWnd = ::CreateWindow(
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
	::SetWindowText(m_hWnd, WINDOW_TITLE_NAME);
	::ShowWindow(m_hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hWnd);
}

void App::InitApp()
{
	// 모듈 초기화
	Graphics::Initialize(m_hWnd);
	Resource::Initialize({ L"Editor", &Graphics::D2D::LoadSprite, &Graphics::D3D::LoadModel });
	Delegates::OnWindowResized.Register(&Graphics::OnWindowResized);

	// ImGui 초기화
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Editor/Imgui.ini";
	io.Fonts->AddFontFromFileTTF("Editor/NanumGothic.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	auto& style{ ImGui::GetStyle() };
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.DockingSeparatorSize = 1.0f;
	ImGui::StyleColorsDark();
	
	// 타이머 초기화
	m_timer.Tick();
}

void App::Update()
{
	float deltaTime{ m_timer.Tick() };
}

void App::Render()
{
	Graphics::D3D::Begin();
	{
		Graphics::ImGui::Begin();
		{
			ImGui::ShowDemoWindow();
		}
		Graphics::ImGui::End();
	}
	Graphics::D3D::End();
	Graphics::D2D::Begin();
	{
	}
	Graphics::D2D::End();
	Graphics::D3D::Present();
}
