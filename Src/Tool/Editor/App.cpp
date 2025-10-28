#include "Pch.h"
#include "App.h"
#include "Delegates.h"
#include "Hierarchy.h"

App::App() :
	m_hWnd{ NULL },
	m_windowSize{ 1280, 720 }
{
	InitWindow();
	InitApp();
	InitImGui();
}

App::~App()
{
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
			Delegates::OnWindowResized.Broadcast(width, height);
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
	Resource::Initialize({ L"Editor", &Graphics::D2D::LoadSprite, &Graphics::D3D::LoadModel });
	Graphics::Initialize(m_hWnd);
	Delegates::OnWindowResized.Bind(&Graphics::OnWindowResized);

	// 싱글톤 초기화
	Hierarchy::Instantiate();

	// 타이머 초기화
	m_timer.Tick();
}

void App::InitImGui()
{
	ImGui::SetCurrentContext(Graphics::ImGui::GetContext());

	auto& io{ ImGui::GetIO() };
	io.IniFilename = "Editor/Imgui.ini";
	io.Fonts->AddFontFromFileTTF("Editor/NanumGothic.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

	// 크기
	{
		auto& style{ ImGui::GetStyle() };
		// Borders
		style.WindowBorderSize = 3.0f;

		// Rounding
		style.FrameRounding = 3.0f;
		style.PopupRounding = 3.0f;
		style.ScrollbarRounding = 3.0f;
		style.GrabRounding = 3.0f;

		// Docking
		style.DockingSeparatorSize = 3.0f;
	}

	// 색깔
	{
		constexpr auto ToRGBA = [](uint32_t argb) constexpr
		{
			ImVec4 color{};
			color.x = ((argb >> 16) & 0xFF) / 255.0f;
			color.y = ((argb >> 8) & 0xFF) / 255.0f;
			color.z = (argb & 0xFF) / 255.0f;
			color.w = ((argb >> 24) & 0xFF) / 255.0f;
			return color;
		};

		constexpr auto Lerp = [](const ImVec4& a, const ImVec4& b, float t) constexpr
		{
			return ImVec4{
				std::lerp(a.x, b.y, t),
				std::lerp(a.y, b.y, t),
				std::lerp(a.z, b.z, t),
				std::lerp(a.w, b.w, t)
			};
		};

		auto colors{ ImGui::GetStyle().Colors };
		colors[ImGuiCol_Text] = ToRGBA(0xFFABB2BF);
		colors[ImGuiCol_TextDisabled] = ToRGBA(0xFF565656);
		colors[ImGuiCol_WindowBg] = ToRGBA(0xFF282C34);
		colors[ImGuiCol_ChildBg] = ToRGBA(0xFF21252B);
		colors[ImGuiCol_PopupBg] = ToRGBA(0xFF2E323A);
		colors[ImGuiCol_Border] = ToRGBA(0xFF2E323A);
		colors[ImGuiCol_BorderShadow] = ToRGBA(0x00000000);
		colors[ImGuiCol_FrameBg] = colors[ImGuiCol_ChildBg];
		colors[ImGuiCol_FrameBgHovered] = ToRGBA(0xFF484C52);
		colors[ImGuiCol_FrameBgActive] = ToRGBA(0xFF54575D);
		colors[ImGuiCol_TitleBg] = colors[ImGuiCol_WindowBg];
		colors[ImGuiCol_TitleBgActive] = colors[ImGuiCol_FrameBgActive];
		colors[ImGuiCol_TitleBgCollapsed] = ToRGBA(0x8221252B);
		colors[ImGuiCol_MenuBarBg] = colors[ImGuiCol_ChildBg];
		colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_PopupBg];
		colors[ImGuiCol_ScrollbarGrab] = ToRGBA(0xFF3E4249);
		colors[ImGuiCol_ScrollbarGrabHovered] = ToRGBA(0xFF484C52);
		colors[ImGuiCol_ScrollbarGrabActive] = ToRGBA(0xFF54575D);
		colors[ImGuiCol_CheckMark] = colors[ImGuiCol_Text];
		colors[ImGuiCol_SliderGrab] = ToRGBA(0xFF353941);
		colors[ImGuiCol_SliderGrabActive] = ToRGBA(0xFF7A7A7A);
		colors[ImGuiCol_Button] = colors[ImGuiCol_SliderGrab];
		colors[ImGuiCol_ButtonHovered] = colors[ImGuiCol_FrameBgActive];
		colors[ImGuiCol_ButtonActive] = colors[ImGuiCol_ScrollbarGrabActive];
		colors[ImGuiCol_Header] = colors[ImGuiCol_ChildBg];
		colors[ImGuiCol_HeaderHovered] = ToRGBA(0xFF353941);
		colors[ImGuiCol_HeaderActive] = colors[ImGuiCol_FrameBgActive];
		colors[ImGuiCol_Separator] = colors[ImGuiCol_WindowBg];
		colors[ImGuiCol_SeparatorHovered] = ToRGBA(0xFF3E4452);
		colors[ImGuiCol_SeparatorActive] = colors[ImGuiCol_SeparatorHovered];
		colors[ImGuiCol_ResizeGrip] = colors[ImGuiCol_Separator];
		colors[ImGuiCol_ResizeGripHovered] = colors[ImGuiCol_SeparatorHovered];
		colors[ImGuiCol_ResizeGripActive] = colors[ImGuiCol_SeparatorActive];
		colors[ImGuiCol_InputTextCursor] = ToRGBA(0xFF528BFF);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBgActive];
		colors[ImGuiCol_TabSelected] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabSelectedOverline] = colors[ImGuiCol_HeaderActive];
		colors[ImGuiCol_TabDimmed] = Lerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabDimmedSelected] = Lerp(colors[ImGuiCol_TabSelected], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4{ 0.50f, 0.50f, 0.50f, 0.00f };
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_ChildBg];
		colors[ImGuiCol_DockingEmptyBg] = colors[ImGuiCol_WindowBg];
		colors[ImGuiCol_PlotLines] = ImVec4{ 0.61f, 0.61f, 0.61f, 1.00f };
		colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 0.43f, 0.35f, 1.00f };
		colors[ImGuiCol_PlotHistogram] = ImVec4{ 0.90f, 0.70f, 0.00f, 1.00f };
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 0.60f, 0.00f, 1.00f };
		colors[ImGuiCol_TableHeaderBg] = colors[ImGuiCol_ChildBg];
		colors[ImGuiCol_TableBorderStrong] = colors[ImGuiCol_SliderGrab];
		colors[ImGuiCol_TableBorderLight] = colors[ImGuiCol_FrameBgActive];
		colors[ImGuiCol_TableRowBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
		colors[ImGuiCol_TableRowBgAlt] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.06f };
		colors[ImGuiCol_TextLink] = ToRGBA(0xFF3F94CE);
		colors[ImGuiCol_TextSelectedBg] = ToRGBA(0xFF243140);
		colors[ImGuiCol_TreeLines] = colors[ImGuiCol_Text];
		colors[ImGuiCol_DragDropTarget] = colors[ImGuiCol_Text];
		colors[ImGuiCol_NavCursor] = colors[ImGuiCol_TextLink];
		colors[ImGuiCol_NavWindowingHighlight] = colors[ImGuiCol_Text];
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4{ 0.80f, 0.80f, 0.80f, 0.20f };
		colors[ImGuiCol_ModalWindowDimBg] = ToRGBA(0xC821252B);
	};
}

void App::Update()
{
	float deltaTime{ m_timer.Tick() };
	if (auto hierarchy{ Hierarchy::GetInstance() })
		hierarchy->Update(deltaTime);
}

void App::Render()
{
	Graphics::D3D::Begin();
	{
		Graphics::ImGui::Begin();
		{
			ImGui::ShowDemoWindow();
			if (auto hierarchy{ Hierarchy::GetInstance() })
				hierarchy->Render();
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
