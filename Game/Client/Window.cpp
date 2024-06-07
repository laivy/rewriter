#include "Stdafx.h"
#include "App.h"
#include "Control.h"
#include "Window.h"

IWindow::IWindow() :
	m_titleBarRect{},
	m_pickPos{},
	m_isPicked{ false },
	m_focusControl{ nullptr }
{
}

void IWindow::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		// 피킹 됐는지 확인
		if (m_titleBarRect.IsContain({ x, y }))
		{
			m_isPicked = true;
			m_pickPos = { x, y };
		}

		// 포커스 UI 초기화
		m_focusControl = nullptr;
		for (const auto& control : m_controls)
			control->SetFocus(false);
		break;
	}
	case WM_LBUTTONUP:
	{
		m_isPicked = false;
		m_pickPos = { 0, 0 };
		break;
	}
	default:
		break;
	}

	for (const auto& control : m_controls)
	{
		if (!control->IsContain({ x, y }))
			continue;

		if (message == WM_LBUTTONDOWN && control->IsEnable())
		{
			control->SetFocus(true);
			m_focusControl = control.get();
		}

		auto pos{ control->GetPosition() };
		control->OnMouseEvent(message, x - pos.x, y - pos.y);
	}
}

void IWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_isFocus)
		return;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			Destroy();
		break;
	}

	if (m_focusControl)
		m_focusControl->OnKeyboardEvent(message, wParam, lParam);
}

void IWindow::Update(float deltaTime)
{
	// 선택된 윈도우 마우스로 옮기기
	if (m_isPicked)
	{
		POINT cursor{};
		::GetCursorPos(&cursor);
		::ScreenToClient(App::hWnd, &cursor);
		SetPosition({
			cursor.x - m_pickPos.x,
			cursor.y - m_pickPos.y
		});
	}
	
	for (const auto& control : m_controls)
		control->Update(deltaTime);
}

void IWindow::Render() const
{
}