#include "Stdafx.h"
#include "WndManager.h"
#include "Wnd.h"
#include "ClientApp.h"
#include "EventManager.h"

WndManager::WndManager()
{
	if (!EventManager::IsInstanced())
		EventManager::Instantiate();

	if (auto em{ EventManager::GetInstance() })
		em->OnSceneChange.Add(std::bind_front(&WndManager::OnSceneChange, this));
}

void WndManager::OnMouseMove(int x, int y)
{
	INT2 cursor{ ClientApp::GetInstance()->GetCursorPosition() };
	for (const auto& w : m_wnds | std::views::filter([](const auto& w) { return w->IsValid(); }))
	{
		INT2 pos{ cursor };
		pos -= w->GetPosition();
		w->OnMouseMove(pos.x, pos.y);
	}
}

void WndManager::OnLButtonUp(int x, int y)
{
	for (const auto& w : m_wnds)
		w->SetPick(false);

	INT2 cursor{ ClientApp::GetInstance()->GetCursorPosition() };
	for (const auto& w : m_wnds | std::views::filter([x, y](const auto& w) { return w->IsValid() && w->IsContain({ x, y }); }))
	{
		INT2 pos{ cursor };
		pos -= w->GetPosition();
		w->OnLButtonUp(pos.x, pos.y);
	}
}

void WndManager::OnLButtonDown(int x, int y)
{
	INT2 cursor{ ClientApp::GetInstance()->GetCursorPosition() };
	Wnd* pickWnd{ nullptr };
	Wnd* focusWnd{ nullptr };
	for (const auto& w : m_wnds | std::views::reverse)
	{
		if (!pickWnd && w->IsInPickArea(cursor))
			pickWnd = w.get();
		else if (!pickWnd && !focusWnd && w->IsContain(cursor))
			focusWnd = w.get();
		w->SetFocus(false);
	}
	if (pickWnd)
	{
		pickWnd->SetPick(true);
		SetTopWnd(pickWnd);
	}
	else if (focusWnd)
	{
		focusWnd->SetFocus(true);
		SetTopWnd(focusWnd);
	}

	for (const auto& w : m_wnds | std::views::filter([x, y](const auto& w) { return w->IsValid() && w->IsContain({ x, y }); }))
	{
		INT2 pos{ cursor };
		pos -= w->GetPosition();
		w->OnLButtonDown(pos.x, pos.y);
	}
}

void WndManager::OnRButtonUp(int x, int y)
{
}

void WndManager::OnRButtonDown(int x, int y)
{
}

void WndManager::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	for (const auto& w : m_wnds)
		w->OnKeyboardEvent(message, wParam, lParam);
}

void WndManager::Update(float deltaTime)
{
	for (const auto& w : m_wnds)
		w->Update(deltaTime);
	RemoveInvalidWnds();
}

void WndManager::Render() const
{
	for (const auto& w : m_wnds)
		w->Render();
}

void WndManager::Clear()
{
	m_wnds.clear();
}

bool WndManager::OnSceneChange(IScene* scene)
{
	if (auto wm{ WndManager::GetInstance() })
	{
		Clear();
		return false;
	}
	return true;
}

void WndManager::SetTopWnd(Wnd* wnd)
{
	if (!wnd)
		return;

	auto it{ std::ranges::find_if(m_wnds, [&](const auto& w) { return w.get() == wnd; })};
	std::rotate(it, it + 1, m_wnds.end());
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
