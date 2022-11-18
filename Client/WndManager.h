#pragma once
#include "GameObject.h"

class Wnd;

class WndManager : public TSingleton<WndManager>
{
public:
	WndManager() = default;
	~WndManager() = default;

	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget);

	void AddWnd(std::unique_ptr<Wnd>& wnd);

	void SetTopWnd(const Wnd* const wnd);

	std::mutex& GetLock() { return m_mutex; }

private:
	std::mutex m_mutex;
	std::list<std::unique_ptr<Wnd>>	m_wnds;
};