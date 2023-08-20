#pragma once
#include "Wnd.h"

class WndManager : public TSingleton<WndManager>
{
public:
	WndManager() = default;
	~WndManager() = default;

	bool OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void Clear();

	template <class T>
	requires std::is_base_of_v<Wnd, T>
	void AddWnd(T* wnd)
	{
		m_wnds.emplace_back(wnd);
	}

	void SetFocusWnd(Wnd* wnd);
	void SetTopWnd(Wnd* wnd);

	// 이벤트 함수들
	void OnSceneChange();

private:
	void RemoveInvalidWnds();

private:
	std::vector<std::unique_ptr<Wnd>> m_wnds;
};