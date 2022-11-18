#include "Stdafx.h"
#include "NytApp.h"
#include "Timer.h"
#include "WndManager.h"
#include "Wnd.h"
#include "MouseWorkerThread.h"
#include "KeyWorkerThread.h"

NytApp::NytApp() : m_hwnd{ NULL }, m_timer{ new Timer }
{
	HRESULT hr{};
	hr = InitD2D();
	
	if (SUCCEEDED(hr))
		hr = InitWnd();

	if (SUCCEEDED(hr))
		hr = CreateDeviceResources();

	assert(SUCCEEDED(hr));

	WndManager::Instantiate();
	if (WndManager::IsInstanced())
	{
		auto wnd1{ std::make_unique<Wnd>(500.0f, 500.0f, 0.0f, 0.0f) };
		WndManager::GetInstance()->AddWnd(wnd1);

		auto wnd2{ std::make_unique<Wnd>(300.0f, 200.0f, 300.0f, 300.0f) };
		WndManager::GetInstance()->AddWnd(wnd2);

		auto wnd3{ std::make_unique<Wnd>(150.0f, 400.0f, 400.0f, 400.0f) };
		WndManager::GetInstance()->AddWnd(wnd3);
	}

	MouseWorkerThread::Instantiate();
	KeyboardWorkerThread::Instantiate();
}

NytApp::~NytApp()
{

}

void NytApp::Run()
{
	MSG msg{};
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

HRESULT NytApp::InitD2D()
{
	HRESULT hr{ S_OK };
	hr = CoInitialize(NULL);

	if (SUCCEEDED(hr))
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_D2DFactory.GetAddressOf());

	if (SUCCEEDED(hr))
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_WICFactory));

	if (SUCCEEDED(hr))
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_DWriteFactory),
			reinterpret_cast<IUnknown**>(m_DWriteFactory.GetAddressOf())
		);

	static const WCHAR msc_fontName[] = L"Verdana";
	static const FLOAT msc_fontSize = 50;
	if (SUCCEEDED(hr))
		hr = m_DWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"",
			&m_textFormat
		);

	if (SUCCEEDED(hr))
	{
		m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		hr = m_D2DFactory->CreatePathGeometry(m_pathGeometry.GetAddressOf());
	}

	ID2D1GeometrySink* pSink{ NULL };
	if (SUCCEEDED(hr))
		hr = m_pathGeometry->Open(&pSink);

	if (SUCCEEDED(hr))
	{
		pSink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);

		pSink->BeginFigure(D2D1::Point2F(0, 0), D2D1_FIGURE_BEGIN_FILLED);

		pSink->AddLine(D2D1::Point2F(200, 0));

		pSink->AddBezier(
			D2D1::BezierSegment(
				D2D1::Point2F(150, 50),
				D2D1::Point2F(150, 150),
				D2D1::Point2F(200, 200))
		);

		pSink->AddLine(D2D1::Point2F(0, 200));

		pSink->AddBezier(
			D2D1::BezierSegment(
				D2D1::Point2F(50, 150),
				D2D1::Point2F(50, 50),
				D2D1::Point2F(0, 0))
		);

		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

		hr = pSink->Close();
	}
	pSink->Release();

	return hr;
}

HRESULT NytApp::InitWnd()
{
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D2DApp";
	RegisterClassEx(&wcex);

	m_hwnd = CreateWindow(
		L"D2DApp",
		L"App",
		WS_OVERLAPPED | WS_SYSMENU | WS_BORDER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1920,
		1080,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
		);

	HRESULT hr = m_hwnd ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
	}
	
	return hr;
}

LRESULT CALLBACK NytApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	NytApp* app{};
	if (message == WM_NCCREATE)
	{
		LPCREATESTRUCT pcs{ reinterpret_cast<LPCREATESTRUCT>(lParam) };
		app = static_cast<NytApp*>(pcs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
		return 1;
	}

	app = reinterpret_cast<NytApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		if (MouseWorkerThread::IsInstanced())
			MouseWorkerThread::GetInstance()->OnMouseEvent(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if (KeyboardWorkerThread::IsInstanced())
			KeyboardWorkerThread::GetInstance()->OnKeyboardEvent(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void NytApp::Update()
{
	m_timer->Tick();
	FLOAT deltaTime{ m_timer->GetDeltaTime() };

	if (WndManager::IsInstanced())
		WndManager::GetInstance()->Update(deltaTime);
}

void NytApp::Render()
{
	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF{ D2D1::ColorF::White });

	if (WndManager::IsInstanced())
		WndManager::GetInstance()->Render(m_renderTarget);

	m_renderTarget->EndDraw();
}

HRESULT NytApp::CreateDeviceResources()
{
	if (m_renderTarget)
		return S_OK;

	HRESULT hr{ S_OK };
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	D2D1_SIZE_U size{ D2D1::SizeU(
		static_cast<UINT>(rc.right - rc.left),
		static_cast<UINT>(rc.bottom - rc.top)
	) };

	// Create a Direct2D render target.
	hr = m_D2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_hwnd, size),
		&m_renderTarget
	);

	if (SUCCEEDED(hr))
		hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &m_blackBrush);

	return hr;
}