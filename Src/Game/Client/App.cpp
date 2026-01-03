#include "Pch.h"
#include "App.h"
#include "Delegates.h"

App::App() :
	m_hWnd{ NULL },
	m_windowSize{ 1600, 900 }
{
	InitWindow();
	InitApp();
}

App::~App()
{
	Resource::Finalize();
	Graphics::Finalize();
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
	if (Graphics::ImGui::WndProc(hWnd, message, wParam, lParam))
		return true;
#endif

	switch (message)
	{
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
	constexpr auto ClassName{ L"App" };
	constexpr auto WindowName{ L"Rewriter" };

	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = ClassName;
	if (!::RegisterClassEx(&wcex))
		return;

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
	// 모듈 초기화
	Graphics::Initialize(m_hWnd);
	Resource::Initialize({ L"Data", &Graphics::LoadSprite });
	Delegates::OnWindowResized.Bind(&Graphics::OnWindowResized);

#ifdef _IMGUI
	// ImGui 초기화
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());
	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Data/Imgui.ini";
	//for (const auto& entry : std::filesystem::directory_iterator{ L"Data/Font" })
	//{
	//	const auto& path{ entry.path() };
	//	if (path.extension() == L".ttf")
	//		io.Fonts->AddFontFromFileTTF(reinterpret_cast<const char*>(path.u8string().c_str()), 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());
	//}
#endif

	// 타이머 초기화
	m_timer.Tick();
}

void App::Update()
{
	const float deltaTime{ m_timer.Tick() };
}

void App::Render()
{
	Graphics::Begin3D();
	{
#ifdef _IMGUI
		Graphics::ImGui::Begin();
		{
			ImGui::ShowDemoWindow();
		}
		Graphics::ImGui::End();
#endif
	}
	Graphics::End3D();
	Graphics::Begin2D();
	{
		// ...
	}
	Graphics::End2D();
	Graphics::Present();
}
