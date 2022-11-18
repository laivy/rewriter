#include "Stdafx.h"
#include "Wnd.h"
#include "Button.h"
#include "NytApp.h"
#include "WndManager.h"

Wnd::Wnd(FLOAT width, FLOAT height, FLOAT x, FLOAT y) :
	m_isValid{ TRUE },
	m_isFocus{ FALSE },
	m_isPick{ FALSE },
	m_pickDelta{ 0.0f, 0.0f }
{
	static bool isFirst{ TRUE };
	if (isFirst)
	{
		// UI 객체의 좌표계는 부모 윈도우 좌표계이다.
		auto button1{ std::make_unique<Button>(80.0f, 20.0f, 0.0f, height * 0.3f) };
		AddUI(button1);

		auto button2{ std::make_unique<Button>(80.0f, 20.0f, 0.0f, height * 0.4f) };
		AddUI(button2);

		isFirst = FALSE;
	}

	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });
}

Wnd::~Wnd()
{
	
}

void Wnd::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_isValid)
		return;

	// 이 경우엔 무조건 픽이 풀린다.
	if (message == WM_LBUTTONUP)
		SetPick(FALSE);
	
	// ui 객체로 이벤트를 넘겨준다.
	// 마우스 좌표는 클라이언트 좌표계 -> 부모 윈도우 좌표계로 바꿔서 전달한다.
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);
	mouse.x -= static_cast<LONG>(m_position.x - m_size.x / 2.0f);
	mouse.y -= static_cast<LONG>(m_position.y - m_size.y / 2.0f);

	for (const auto& ui : m_ui)
		ui->OnMouseEvent(hWnd, message, mouse.x, mouse.y);
}

void Wnd::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_isFocus || !m_isValid)
		return;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			Destroy();
		break;
	}
}

void Wnd::OnButtonClicked(INT id)
{
	OutputDebugString((std::to_wstring(id) + L"\n").c_str());
}

void Wnd::Update(FLOAT deltaTime)
{
	if (!m_isValid) return;

	// 선택된 윈도우 마우스로 옮기기
	if (m_isPick)
	{
		FLOAT2 delta{ GetPickedDelta() };
		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);
		SetPosition(FLOAT2{ static_cast<FLOAT>(mouse.x + delta.x), static_cast<FLOAT>(mouse.y + delta.y) });
	}
}

void Wnd::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget)
{
	if (!m_isValid) return;

	ID2D1SolidColorBrush* brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));

	// 포커스 되어있다면 테두리
	if (m_isFocus)
	{
		ID2D1SolidColorBrush* focusBrush;
		renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Aqua }, &focusBrush);
		renderTarget->DrawRectangle(
			D2D1::RectF(-1 * m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f),
			focusBrush,
			10.0f
		);
	}

	// 창
	renderTarget->FillRectangle(
		D2D1::RectF(-m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f),
		brush
	);
	brush->Release();

	// 타이틀
	ID2D1SolidColorBrush* titleBrush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::CadetBlue }, &titleBrush);
	renderTarget->FillRectangle(
		D2D1::RectF(-m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, -m_size.y / 2.0f + 15.0f),
		titleBrush
	);
	titleBrush->Release();

	// 버튼
	for (const auto& b : m_ui)
		b->Render(renderTarget);
}

void Wnd::Destroy()
{
	m_isValid = FALSE;
}

template <typename T>
void Wnd::AddUI(std::unique_ptr<T>& ui)
{
	std::unique_ptr<UI> _ui{ static_cast<UI*>(ui.release()) };
	_ui->SetParent(this);
	m_ui.push_back(std::move(_ui));
}

void Wnd::SetFocus(BOOL isFocus)
{
	m_isFocus = isFocus;
}

void Wnd::SetPick(BOOL isPick)
{
	m_isPick = isPick;
	if (isPick)
	{
		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);

		m_pickDelta.x = m_position.x - mouse.x;
		m_pickDelta.y = m_position.y - mouse.y;
	}
	else
	{
		m_pickDelta = FLOAT2{ 0.0f, 0.0f };
	}
}

std::mutex& Wnd::GetLock()
{
	return m_mutex;
}

BOOL Wnd::GetIsValid() const
{
	return m_isValid;
}

BOOL Wnd::GetIsFocus() const
{
	return m_isFocus;
}

BOOL Wnd::GetIsPick() const
{
	return m_isPick;
}

FLOAT2 Wnd::GetPickedDelta() const
{
	return m_pickDelta;
}