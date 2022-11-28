#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "NytApp.h"

void WndManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(NytApp::GetInstance()->GetHwnd(), &mouse);

	// 어떤 창이 선택됐는지 체크한다.
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		Wnd* focusWnd{ nullptr };
		Wnd* pickWnd{ nullptr };
		FLOAT2 pos{ static_cast<FLOAT>(mouse.x), static_cast<FLOAT>(mouse.y) };

		for (const auto& w : m_wnds)
		{
			// 창의 어디든 클릭됐는지 확인한다.
			RECTF rect{ w->GetRect() };
			if (Util::IsContain(rect, pos))
				focusWnd = w.get();

			// 이 창의 타이틀 부분이 클릭됐는지 확인한다.
			rect.bottom = rect.top + 15.0f;
			if (Util::IsContain(rect, pos))
				pickWnd = w.get();

			w->SetFocus(FALSE);
			w->SetPick(FALSE);
		}

		if (focusWnd)
		{
			focusWnd->SetFocus(TRUE);
			std::unique_lock lock{ m_mutex };
			SetTopWnd(focusWnd);
		}
		if (pickWnd)
			pickWnd->SetPick(TRUE);
		break;
	}
	case WM_LBUTTONUP:
		for (const auto& w : m_wnds)
			w->SetPick(FALSE);
		break;
	}

	for (const auto& w : m_wnds)
	{
		if (!w->IsValid())
			continue;

		// 윈도우 객체들에게는 클라이언트 좌표계 -> 해당 윈도우 좌표계로 바꿔서 전달한다.
		POINT clientPos{ mouse };
		FLOAT2 wndPos{ w->GetPosition() };
		clientPos.x -= static_cast<LONG>(wndPos.x);
		clientPos.y -= static_cast<LONG>(wndPos.y);
		w->OnMouseEvent(hWnd, message, clientPos.x, clientPos.y);
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

void WndManager::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget)
{
	std::unique_lock lock{ m_mutex };
	for (const auto& w : m_wnds)
		w->Render(renderTarget);
}

void WndManager::SetTopWnd(const Wnd* const wnd)
{
	// 이미 락을 잡고 들어온다.

	if (!wnd)
		return;

	auto it = std::ranges::find_if(m_wnds, [&](const std::unique_ptr<Wnd>& e) { return e.get() == wnd; });
	if (it == m_wnds.end())
		return;

	m_wnds.push_back(std::move(*it));
	m_wnds.erase(it);
}

