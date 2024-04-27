#include "Stdafx.h"
#include "Window.h"

void IWindow::OnMouseEvent(UINT message, int x, int y)
{
}

void IWindow::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_isValid || !m_isFocus)
		return;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			Destroy();
		break;
	}

	for (const auto& ui : m_userInterfaces)
		ui->OnKeyboardEvent(message, wParam, lParam);
}

void IWindow::Update(float deltaTime)
{
	if (!m_isValid)
		return;

	// 선택된 윈도우 마우스로 옮기기
	//if (m_isPick)
	//{
	//	INT2 mouse{ App::GetInstance()->GetCursorPosition() };
	//	SetPosition({ mouse.x + m_pickDelta.x, mouse.y + m_pickDelta.y });
	//}
	
	for (const auto& ui : m_userInterfaces)
		ui->Update(deltaTime);
}

void IWindow::Render() const
{
	if (!m_isValid)
		return;

	// UI
	for (const auto& ui : m_userInterfaces)
		ui->Render();
}