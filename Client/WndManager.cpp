#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "GameApp.h"

void WndManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 어떤 창이 선택됐는지 체크한다.
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		for (const auto& w : m_wnds)
			w->SetFocus(FALSE);
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
}

void WndManager::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (const auto& w : m_wnds)
		w->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void WndManager::Update(FLOAT deltaTime)
{
	std::unique_lock lock{ m_mutex };

	for (const auto& w : m_wnds)
		w->Update(deltaTime);

	auto removeCount{ m_wnds.remove_if([](const std::unique_ptr<Wnd>& w) { return !w->IsValid(); }) };
	if (removeCount > 0 && m_wnds.size() > 0)
	{
		m_wnds.back()->SetFocus(TRUE);
		SetTopWnd(m_wnds.back().get());
	}

	// 윈도우 객체 추가
	if (!m_addWnds.empty())
	{
		std::ranges::move(m_addWnds, std::back_inserter(m_wnds));
		m_addWnds.clear();
	}
}

void WndManager::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	std::unique_lock lock{ m_mutex };
	for (const auto& w : m_wnds)
		w->Render(d2dContext);
}

void WndManager::RemoveAllWnd()
{
	std::unique_lock wndLock{ m_mutex, std::defer_lock };
	std::unique_lock addWndLock{ m_addWndsMutex, std::defer_lock };
	std::lock(wndLock, addWndLock);

	m_wnds.clear();
	m_addWnds.clear();
}

void WndManager::SetWndFocus(Wnd* const focusWnd)
{
	std::unique_lock lock{ m_mutex };
	for (const auto& w : m_wnds)
		w->SetFocus(FALSE);
	if (focusWnd)
		focusWnd->SetFocus(TRUE);
}

void WndManager::SetTopWnd(const Wnd* const wnd)
{
	// 이미 락을 잡고 들어온다.
	if (!wnd) return;

	auto it = std::ranges::find_if(m_wnds, [&](const std::unique_ptr<Wnd>& e) { return e.get() == wnd; });
	if (it == m_wnds.end())
		return;

	m_wnds.push_back(std::move(*it));
	m_wnds.erase(it);
}