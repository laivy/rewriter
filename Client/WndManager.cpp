#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "NytApp.h"

void WndManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 어떤 창이 선택됐는지 체크한다.
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		POINT mouse;
		GetCursorPos(&mouse);
		ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);

		Wnd* focusWnd{ nullptr };
		Wnd* pickWnd{ nullptr };
		for (const auto& w : m_wnds)
		{
			// 창의 어디든 클릭됐는지 확인한다.
			RECTF rect{ w->GetRect() };
			if (Util::IsContain(rect, mouse))
				focusWnd = w.get();

			// 이 창의 타이틀 부분이 클릭됐는지 확인한다.
			rect.bottom = rect.top + 15.0f;
			if (Util::IsContain(rect, mouse))
				pickWnd = w.get();

			w->SetFocus(FALSE);
			w->SetPick(FALSE);
		}

		if (focusWnd)
		{
			focusWnd->SetFocus(TRUE);
			std::unique_lock lock{ GetLock() };
			SetTopWnd(focusWnd);
		}
		if (pickWnd)
			pickWnd->SetPick(TRUE);
		break;
	}
	default:
		break;
	}

	for (const auto& w : m_wnds)
		w->OnMouseEvent(hWnd, message, wParam, lParam);
}

void WndManager::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	for (const auto& w : m_wnds)
		w->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void WndManager::Update(FLOAT deltaTime)
{
	std::unique_lock lock{ GetLock() };
	for (const auto& w : m_wnds)
		w->Update(deltaTime);

	auto removeCount{ m_wnds.remove_if([](const std::unique_ptr<Wnd>& w) { return !w->GetIsValid(); }) };
	if (removeCount > 0 && m_wnds.size() > 0)
	{
		m_wnds.back()->SetFocus(TRUE);
		SetTopWnd(m_wnds.back().get());
	}
}

void WndManager::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget)
{
	std::unique_lock lock{ GetLock() };
	for (const auto& w : m_wnds)
		w->Render(renderTarget);
}

void WndManager::SetTopWnd(const Wnd* const wnd)
{
	if (!wnd)
		return;

	auto it = std::ranges::find_if(m_wnds, [&](const std::unique_ptr<Wnd>& e) { return e.get() == wnd; });
	if (it == m_wnds.end())
		return;

	m_wnds.push_back(std::move(*it));
	m_wnds.erase(it);
}

void WndManager::AddWnd(std::unique_ptr<Wnd>& wnd)
{
	for (const auto& w : m_wnds)
		w->SetFocus(FALSE);
	wnd->SetFocus(TRUE);
	m_wnds.push_back(std::move(wnd));
}
