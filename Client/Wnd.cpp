#include "Stdafx.h"
#include "Wnd.h"
#include "WndManager.h"
#include "GameApp.h"
#include "Button.h"
#include "EditCtrl.h"

Wnd::Wnd(const INT2& size) :
	m_isPick{ false },
	m_pickDelta{ 0, 0 }
{
	SetSize(size);
	m_pickArea = { 0, 0, size.x, DEFAULT_PICK_AREA_HEIGHT };
}

void Wnd::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{
	if (!m_isValid)
		return;

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		IUserInterface* focusUI{ nullptr };
		for (const auto& ui : m_userInterfaces)
		{
			if (ui->IsContain({ x, y }))
				focusUI = ui.get();
			ui->SetFocus(false);
		}
		if (focusUI)
			focusUI->SetFocus(true);
		break;
	}
	default:
		break;
	}

	// UI 객체들에게 윈도우 좌표계 -> UI 좌표계로 바꿔서 전달한다.
	for (const auto& ui : m_userInterfaces)
	{
		if (!ui->IsValid())
			continue;

		if (!ui->IsContain({ x, y }))
			continue;

		INT2 pos{ ui->GetPosition(Pivot::LEFTTOP) };
		ui->OnMouseEvent(hWnd, message, x - pos.x, y - pos.y);
	}
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

	for (const auto& ui : m_userInterfaces)
		ui->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void Wnd::OnButtonClicked(INT id)
{

}

void Wnd::Update(FLOAT deltaTime)
{
	if (!m_isValid)
		return;

	// 선택된 윈도우 마우스로 옮기기
	if (m_isPick)
	{
		INT2 mouse{ GameApp::GetInstance()->GetCursorPosition() };
		SetPosition({ mouse.x + m_pickDelta.x, mouse.y + m_pickDelta.y });
	}
	
	for (const auto& ui : m_userInterfaces)
		ui->Update(deltaTime);
}

void Wnd::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext)
{
	if (!m_isValid)
		return;

	ComPtr<ID2D1SolidColorBrush> brush{};
	d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Black }, &brush);
	d2dContext->SetTransform(D2D1::Matrix3x2F::Translation(m_position.x, m_position.y));

	// 포커스 되어있다면 테두리를 그린다.
	if (m_isFocus)
	{
		ComPtr<ID2D1SolidColorBrush> focusBrush{};
		d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::Aqua }, &focusBrush);
		d2dContext->DrawRectangle(RECTF{ -m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f }, focusBrush.Get(), 10.0f);
	}

	// 창
	d2dContext->FillRectangle(RECTF{ -m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, m_size.y / 2.0f }, brush.Get());

	// 타이틀
	ComPtr<ID2D1SolidColorBrush> titleBrush{};
	d2dContext->CreateSolidColorBrush(D2D1::ColorF{ D2D1::ColorF::CadetBlue }, &titleBrush);
	d2dContext->FillRectangle(RECTF{ -m_size.x / 2.0f, -m_size.y / 2.0f, m_size.x / 2.0f, -m_size.y / 2.0f + 15.0f }, titleBrush.Get());

	// UI
	for (const auto& ui : m_userInterfaces)
		ui->Render(d2dContext);
}

void Wnd::SetUIFocus(IUserInterface* focusUI)
{
	for (const auto& ui : m_userInterfaces)
		ui->SetFocus(false);
	if (focusUI)
		focusUI->SetFocus(true);
}

void Wnd::SetFocus(bool isFocus)
{
	IUserInterface::SetFocus(isFocus);
	if (!m_isFocus)
	{
		for (const auto& ui : m_userInterfaces)
			ui->SetFocus(false);
		SetPick(false); // pick은 focus의 상위호환
	}
}

void Wnd::SetPick(bool isPick)
{
	m_isPick = isPick;
	if (isPick)
	{
		INT2 mouse{ GameApp::GetInstance()->GetCursorPosition() };
		m_pickDelta = m_position - mouse;
		SetFocus(true); // pick은 focus의 상위호환
	}
	else
	{
		m_pickDelta = { 0, 0 };
	}
}

bool Wnd::IsPick() const
{
	return m_isPick;
}

bool Wnd::IsInPickArea(const INT2& point)
{
	// point는 윈도우 좌표계의 좌표
	RECTI pickArea{ m_pickArea };
	pickArea.Offset(m_position.x - m_size.x / 2, m_position.y - m_size.y / 2);
	return pickArea.IsContain(point);
}