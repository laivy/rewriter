#include "Pch.h"
#include "App.h"
#include "Delegates.h"
#include "Desktop.h"
#include "Hierarchy.h"

App::App() :
	m_hWnd{ NULL },
	m_windowSize{ 1280, 720 }
{
	InitWindow();
	InitApp();
}

App::~App()
{
	Desktop::Destroy();
	Hierarchy::Destroy();
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
		return 0;

	App* app{ reinterpret_cast<App*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
	switch (message)
	{
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			int width{ GET_X_LPARAM(lParam) };
			int height{ GET_Y_LPARAM(lParam) };
			if (width == 0 && height == 0)
				assert(false);
			Delegates::OnWindowResized.Broadcast(width, height);
		}
		return 0;
	}
	case WM_NCCREATE:
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcs->lpCreateParams));
		return TRUE;
	}
	default:
		break;
	}
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void App::InitWindow()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = WindowName;
	if (!::RegisterClassEx(&wcex))
		return;

	// 화면 최대 크기로 윈도우 생성
	RECT rect{ 0, 0, m_windowSize.x, m_windowSize.y };
	if (!::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE))
		return;

	m_hWnd = ::CreateWindow(
		wcex.lpszClassName,
		WindowName,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		wcex.hInstance,
		this
	);
	if (!m_hWnd)
		return;

	::SetWindowText(m_hWnd, WindowName);
}

void App::InitApp()
{
	// 라이브러리 초기화
	Resource::Initialize({ L"Editor", &Graphics::D2D::LoadSprite, &Graphics::D3D::LoadModel });
	Graphics::Initialize(m_hWnd);
	Delegates::OnWindowResized.Bind(&Graphics::OnWindowResized);

	// ImGui 초기화
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());
	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Editor/Imgui.ini";
	for (const auto& entry : std::filesystem::directory_iterator{ L"Editor/Font" })
	{
		const auto& font{ entry.path() };
		if (font.extension() == L".ttf")
			io.Fonts->AddFontFromFileTTF(font.string().c_str(), 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	}

	// 싱글톤 초기화
	Desktop::Instantiate();
	Hierarchy::Instantiate();

	// 타이머 초기화
	m_timer.Tick();
}

void App::Update()
{
	float deltaSeconds{ m_timer.Tick() };
	if (auto desktop{ Desktop::GetInstance() })
		desktop->Update(deltaSeconds);
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->Update(deltaSeconds);
}

void App::Render()
{
	Graphics::D3D::Begin();
	{
		Graphics::ImGui::Begin();
		{
			if (auto desktop{ Desktop::GetInstance() })
				desktop->Render();
			if (auto hierarchy{ Hierarchy::GetInstance() })
				hierarchy->Render();
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
