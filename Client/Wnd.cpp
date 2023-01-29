#include "Stdafx.h"
#include "Wnd.h"
#include "WndManager.h"
#include "NytApp.h"
#include "Button.h"
#include "EditCtrl.h"

Wnd::Wnd(FLOAT width, FLOAT height, FLOAT x, FLOAT y) :
	m_isFocus{ FALSE },
	m_isPick{ FALSE },
	m_pickDelta{ 0.0f, 0.0f }
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ x, y });
}

void Wnd::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{
	for (const auto& ui : m_ui)
		ui->OnMouseEvent(hWnd, message, x, y);
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

	auto wnd{ std::make_unique<Wnd>(150.0f, 400.0f, 400.0f, 400.0f) };
	WndManager::GetInstance()->AddWnd(wnd);
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

	for (const auto& ui : m_ui)
		ui->Update(deltaTime);
}

void Wnd::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget)
{
	if (!m_isValid) return;

	ComPtr<ID2D1SolidColorBrush> brush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &brush);
	renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));

	// 포커스 되어있다면 테두리를 그린다.
	if (m_isFocus)
	{
		ID2D1SolidColorBrush* focusBrush;
		renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Aqua }, &focusBrush);
		renderTarget->DrawRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, m_size.y), focusBrush, 10.0f);
	}

	// 창
	renderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, m_size.y), brush.Get());

	// 타이틀
	ComPtr<ID2D1SolidColorBrush> titleBrush{};
	renderTarget->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::CadetBlue }, &titleBrush);
	renderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, m_size.x, 15.0f), titleBrush.Get());

	// UI
	for (const auto& ui : m_ui)
		ui->Render(renderTarget);
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

BOOL Wnd::IsValid() const
{
	return m_isValid;
}

BOOL Wnd::IsFocus() const
{
	return m_isFocus;
}

BOOL Wnd::IsPick() const
{
	return m_isPick;
}

FLOAT2 Wnd::GetPickedDelta() const
{
	return m_pickDelta;
}