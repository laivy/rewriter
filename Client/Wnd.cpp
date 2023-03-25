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

	for (const auto& ui : m_ui)
		ui->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void Wnd::OnButtonClicked(INT id)
{

}

void Wnd::Update(FLOAT deltaTime)
{
	if (!m_isValid) return;

	// 선택된 윈도우 마우스로 옮기기
	if (m_isPick)
	{
		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);
		SetPosition(FLOAT2{ static_cast<FLOAT>(mouse.x + m_pickDelta.x), static_cast<FLOAT>(mouse.y + m_pickDelta.y) });
	}
	
	for (const auto& ui : m_ui)
		ui->Update(deltaTime);
}

void Wnd::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext)
{
	if (!m_isValid) return;

	ComPtr<ID2D1SolidColorBrush> brush{};
	d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &brush);
	d2dContext->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));

	// 포커스 되어있다면 테두리를 그린다.
	if (m_isFocus)
	{
		ComPtr<ID2D1SolidColorBrush> focusBrush;
		d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Aqua }, &focusBrush);
		d2dContext->DrawRectangle(RECTF{ 0.0f, 0.0f, m_size.x, m_size.y }, focusBrush.Get(), 10.0f);
	}

	// 창
	d2dContext->FillRectangle(RECTF{ 0.0f, 0.0f, m_size.x, m_size.y }, brush.Get());

	// 타이틀
	ComPtr<ID2D1SolidColorBrush> titleBrush{};
	d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::CadetBlue }, &titleBrush);
	d2dContext->FillRectangle(RECTF{ 0.0f, 0.0f, m_size.x, 15.0f }, titleBrush.Get());

	// UI
	for (const auto& ui : m_ui)
		ui->Render(d2dContext);
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