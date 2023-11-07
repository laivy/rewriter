#include "Stdafx.h"
#include "Wnd.h"
#include "WndManager.h"
#include "ClientApp.h"
#include "Button.h"
#include "EditCtrl.h"

Wnd::Wnd(const INT2& size) :
	m_isPick{ false },
	m_pickDelta{ 0, 0 }
{
	SetSize(size);
	m_pickArea = { 0, 0, size.x, DEFAULT_PICK_AREA_HEIGHT };
}

void Wnd::OnMouseMove(int x, int y)
{
	for (const auto& ui : m_userInterfaces
		| std::views::filter([](const auto& ui) { return ui->IsValid(); }))
	{
		INT2 pos{ ui->GetPosition(Pivot::LEFTTOP) };
		ui->OnMouseMove(x - pos.x, y - pos.y);
	}
}

void Wnd::OnLButtonUp(int x, int y)
{
	for (const auto& ui : m_userInterfaces
		| std::views::filter([](const auto& ui) { return ui->IsValid(); }))
	{
		INT2 pos{ ui->GetPosition(Pivot::LEFTTOP) };
		ui->OnLButtonUp(x - pos.x, y - pos.y);
	}
}

void Wnd::OnLButtonDown(int x, int y)
{
	IUserInterface* focusUI{ nullptr };
	for (const auto& ui : m_userInterfaces | std::views::reverse)
	{
		if (!focusUI && ui->IsContain({ x, y }))
			focusUI = ui.get();
		ui->SetFocus(false);
	}
	if (focusUI)
		focusUI->SetFocus(true);

	for (const auto& ui : m_userInterfaces
		| std::views::filter([](const auto& ui) { return ui->IsValid(); }))
	{
		INT2 pos{ ui->GetPosition(Pivot::LEFTTOP) };
		ui->OnLButtonDown(x - pos.x, y - pos.y);
	}
}

void Wnd::OnRButtonUp(int x, int y)
{
}

void Wnd::OnRButtonDown(int x, int y)
{
}

void Wnd::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
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
		ui->OnKeyboardEvent(message, wParam, lParam);
}

void Wnd::OnButtonClick(ButtonID id)
{
}

void Wnd::Update(float deltaTime)
{
	if (!m_isValid)
		return;

	// 선택된 윈도우 마우스로 옮기기
	if (m_isPick)
	{
		INT2 mouse{ ClientApp::GetInstance()->GetCursorPosition() };
		SetPosition({ mouse.x + m_pickDelta.x, mouse.y + m_pickDelta.y });
	}
	
	for (const auto& ui : m_userInterfaces)
		ui->Update(deltaTime);
}

void Wnd::Render() const
{
	if (!m_isValid)
		return;

	// UI
	for (const auto& ui : m_userInterfaces)
		ui->Render();
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
		INT2 mouse{ ClientApp::GetInstance()->GetCursorPosition() };
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
	RECTI rect{ m_pickArea };
	rect.Offset(m_position.x, m_position.y);
	return rect.IsContain(point);
}