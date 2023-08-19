#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "GameApp.h"

WndManager::WndManager() : m_focusWnd{ nullptr }
{

}

bool WndManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 어떤 창이 선택됐는지 체크한다.
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		for (const auto& w : m_wnds)
			w->SetFocus(FALSE);
		m_focusWnd = nullptr;
		break;
	}
	case WM_LBUTTONUP:
		for (const auto& w : m_wnds)
			w->SetPick(FALSE);
		break;
	}

	// 윈도우 객체들에게 클라이언트 좌표계 -> 해당 윈도우 좌표계로 바꿔서 전달한다.
	for (const auto& w : m_wnds)
	{
		if (!w->IsValid())
			continue;

		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(GameApp::GetInstance()->GetHwnd(), &mouse);

		FLOAT2 pos{ w->GetPosition() };
		mouse.x -= static_cast<LONG>(pos.x);
		mouse.y -= static_cast<LONG>(pos.y);
		w->OnMouseEvent(hWnd, message, mouse.x, mouse.y);
	}

	return false;
}

bool WndManager::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (const auto& w : m_wnds)
		w->OnKeyboardEvent(hWnd, message, wParam, lParam);
	return false;
}

void WndManager::Update(FLOAT deltaTime)
{
	for (const auto& w : m_wnds)
		w->Update(deltaTime);
	RemoveInvalidWnds();
}

void WndManager::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	for (const auto& w : m_wnds)
		w->Render(d2dContext);
}

void WndManager::Clear()
{
	m_wnds.clear();
}

void WndManager::SetFocusWnd(Wnd* wnd)
{
	for (const auto& w : m_wnds)
		w->SetFocus(FALSE);
	if (wnd)
		wnd->SetFocus(TRUE);
	m_focusWnd = wnd;
}

void WndManager::SetTopWnd(Wnd* wnd)
{
	if (!wnd)
		return;

	auto it = std::ranges::find_if(m_wnds, [&](const std::unique_ptr<Wnd>& e) { return e.get() == wnd; });
	if (it == m_wnds.end())
		return;

	//std::rotate(it, it + 1, m_wnds.end());
	m_wnds.push_back(std::move(*it));
	m_wnds.erase(it);
}

Wnd* WndManager::GetFocusWnd() const
{
	return m_focusWnd;
}

void WndManager::OnSceneChange()
{
	Clear();
}

void WndManager::RemoveInvalidWnds()
{
	auto count{ std::erase_if(m_wnds, [](const auto& w) { return !w->IsValid(); }) };
	if (count > 0 && !m_wnds.empty())
	{
		m_wnds.back()->SetFocus(TRUE);
		SetTopWnd(m_wnds.back().get());
	}
}
