#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "GameApp.h"

bool WndManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT2 mouse{ GameApp::GetInstance()->GetCursorPosition() };

	// 어떤 창이 선택됐는지 체크한다.
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		Wnd* pickWnd{ nullptr };
		Wnd* focusWnd{ nullptr };
		for (const auto& w : m_wnds)
		{
			if (w->IsInPickArea(mouse))
				pickWnd = w.get();
			else if (w->IsContain(mouse))
				focusWnd = w.get();
			w->SetFocus(false);
		}
		if (pickWnd)
			pickWnd->SetPick(true);
		else if (focusWnd)
			focusWnd->SetFocus(true);
		break;
	}
	case WM_LBUTTONUP:
	{
		for (const auto& w : m_wnds)
			w->SetPick(false);
		break;
	}
	default:
		break;
	}

	// 윈도우 객체들에게 클라이언트 좌표계 -> 해당 윈도우 좌표계로 바꿔서 전달한다.
	for (const auto& w : m_wnds)
	{
		if (!w->IsValid())
			continue;

		if (!w->IsContain({ mouse.x, mouse.y }))
			continue;

		INT2 pos{ w->GetPosition() };
		mouse.x -= pos.x - w->GetSize().x / 2;
		mouse.y -= pos.y - w->GetSize().y / 2;
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
		w->SetFocus(false);
	if (wnd)
		wnd->SetFocus(true);
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

void WndManager::OnSceneChange()
{
	Clear();
}

void WndManager::RemoveInvalidWnds()
{
	auto count{ std::erase_if(m_wnds, [](const auto& w) { return !w->IsValid(); }) };
	if (count > 0 && !m_wnds.empty())
	{
		m_wnds.back()->SetFocus(true);
		SetTopWnd(m_wnds.back().get());
	}
}
