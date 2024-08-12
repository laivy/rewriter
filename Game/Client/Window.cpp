#include "Stdafx.h"
#include "App.h"
#include "Control.h"
#include "Window.h"

IWindow::IWindow() :
	m_titleBarRect{},
	m_pickPos{},
	m_isPicked{ false }
{
}

void IWindow::OnMouseLeave(int x, int y)
{
	if (auto control{ m_mouseOverControl.lock() })
		control->OnMouseLeave(x, y);
	m_mouseOverControl.reset();
}

void IWindow::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
	{
		UpdateMouseOverControl(x, y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// 피킹 됐는지 확인
		if (m_titleBarRect.IsContain({ x, y }))
		{
			m_isPicked = true;
			m_pickPos = { x, y };
		}

		UpdateFocusControl(x, y);
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
		if (!control->IsContain({x, y}))
			continue;

		auto pos{ control->GetPosition() };
		control->OnMouseEvent(message, x - pos.x, y - pos.y);
	}
}

void IWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			Destroy();
		break;
	}

	if (auto c{ m_focusControl.lock() })
		c->OnKeyboardEvent(message, wParam, lParam);
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
	for (const auto& control : m_controls)
		control->Render();
}

void IWindow::Register(const std::shared_ptr<IControl>& control)
{
	m_controls.push_back(control);
}

void IWindow::UpdateMouseOverControl(int x, int y)
{
	auto rit{ std::ranges::find_if(std::views::reverse(m_controls), [x, y](const auto& control) { return control->IsContain({ x, y }); }) };
	if (rit == m_controls.rend())
	{
		if (auto control{ m_mouseOverControl.lock() })
			control->OnMouseLeave(x, y);
		m_mouseOverControl.reset();
		return;
	}

	auto control{ m_mouseOverControl.lock() };
	if (control != *rit)
	{
		if (control)
			control->OnMouseLeave(x, y);
		m_mouseOverControl = *rit;
		(*rit)->OnMouseEnter(x, y);
	}
}

void IWindow::UpdateFocusControl(int x, int y)
{
	// 포커스 컨트롤 갱신
	m_focusControl.reset();
	for (const auto& control : m_controls)
	{
		if (control->IsEnable() && control->IsContain({ x, y }))
		{
			control->SetFocus(true);
			m_focusControl = control;
		}
		else
		{
			control->SetFocus(false);
		}
	}
}
