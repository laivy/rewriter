#pragma once
#include "GameObject.h"
#include "Wnd.h"

class WndManager : public TSingleton<WndManager>
{
public:
	WndManager() = default;
	~WndManager() = default;

	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget);

	template <typename T>
	void AddWnd(std::unique_ptr<T>& wnd)
	{
		std::unique_ptr<Wnd> _wnd{ static_cast<Wnd*>(wnd.release()) };
		for (const auto& w : m_wnds)
			w->SetFocus(FALSE);
		_wnd->SetFocus(TRUE);
		m_wnds.push_back(std::move(_wnd));
	}

	void SetTopWnd(const Wnd* const wnd);

	std::mutex& GetLock() { return m_mutex; }

private:
	std::mutex m_mutex;
	std::list<std::unique_ptr<Wnd>>	m_wnds;
};