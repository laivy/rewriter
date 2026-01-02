#include "Pch.h"
#include "App.h"
#include "Delegates.h"
#include "Desktop.h"
#include "Explorer.h"
#include "Hierarchy.h"
#include "Inspector.h"

App::App() :
	m_hWnd{ NULL },
	m_windowSize{ 1600, 900 }
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
	Resource::Finalize();
	Graphics::Finalize();
}

void App::Run()
{
	bool done{ false };
	while (!done)
	{
		MSG msg{};
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
				continue;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if (done)
			break;

		Update();
		Render();
	}
}

LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::ImGui::WndProc(hWnd, message, wParam, lParam))
		return true;

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
			const int width{ GET_X_LPARAM(lParam) };
			const int height{ GET_Y_LPARAM(lParam) };
			Delegates::OnWindowResized.Broadcast(Int2{ width, height });
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
	constexpr auto ClassName{ L"App" };
	constexpr auto WindowName{ L"Editor" };

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
	// 라이브러리 초기화
	Graphics::Initialize(m_hWnd);
	Resource::Initialize({ L"Editor", &Graphics::LoadSprite });
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
	Explorer::Instantiate();
	Hierarchy::Instantiate();
	Inspector::Instantiate();

	// 타이머 초기화
	m_timer.Tick();
}

void App::Update()
{
	float deltaSeconds{ m_timer.Tick() };
	if (auto desktop{ Desktop::GetInstance() })
		desktop->Update(deltaSeconds);
	if (auto explorer{ Explorer::GetInstance() })
		explorer->Update(deltaSeconds);
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->Update(deltaSeconds);
	if (auto inspector{ Inspector::GetInstance() })
		inspector->Update(deltaSeconds);
}

void App::Render()
{
	Graphics::Begin3D();
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
			ImGui::ShowDemoWindow();
		}
		Graphics::ImGui::End();
	}
	Graphics::End3D();
	Graphics::Begin2D();
	{
		// ...
	}
	Graphics::End2D();
	Graphics::Present();
}
