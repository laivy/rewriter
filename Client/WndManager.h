#pragma once
#include "Wnd.h"

class WndManager : public TSingleton<WndManager>
{
public:
	WndManager() = default;
	~WndManager();

	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	template <class T>
	requires std::is_base_of_v<Wnd, T>
	void AddWnd(T* wnd)
	{
		std::unique_lock lock{ m_addWndsMutex };
		for (const auto& w : m_wnds)
			w->SetFocus(FALSE);
		for (const auto& w : m_addWnds)
			w->SetFocus(FALSE);
		wnd->SetFocus(TRUE);
		m_addWnds.emplace_back(wnd);
	}

	void SetWndFocus(Wnd* const focusWnd);
	void SetTopWnd(const Wnd* const wnd);

	std::mutex& GetLock() { return m_mutex; }

private:
	mutable std::mutex m_mutex;
	std::list<std::unique_ptr<Wnd>>	m_wnds;

	// 다음 업데이트 때 추가될 윈도우 객체
	std::mutex m_addWndsMutex;
	std::vector<std::unique_ptr<Wnd>> m_addWnds;
};