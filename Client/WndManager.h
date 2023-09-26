#pragma once
#include "Wnd.h"

class IScene;

class WndManager : public TSingleton<WndManager>
{
public:
	WndManager();
	~WndManager() = default;

	void OnMouseMove(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnRButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void Clear();

	template <class T>
	requires std::is_base_of_v<Wnd, T>
	void AddWnd(T* wnd)
	{
		m_wnds.emplace_back(wnd);
	}

private:
	bool OnSceneChange(IScene* scene);

	void SetTopWnd(Wnd* wnd);
	void RemoveInvalidWnds();

private:
	std::vector<std::unique_ptr<Wnd>> m_wnds;
};