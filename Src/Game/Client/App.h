#pragma once
#include "Common/Timer.h"

class App : public TSingleton<App>
{
public:
	App();
	~App();

	void Run();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitWindow();
	void InitApp();

	void Update();
	void Render();

public:
	static inline Delegate<UINT, WPARAM, LPARAM> OnKeyboardEvent;
	static inline Delegate<UINT, int, int> OnMouseEvent;
	static inline Delegate<int, int> OnResize;

	static constexpr auto WINDOW_TITLE_NAME{ L"REWRITER" };
	static inline HWND hWnd{ NULL };
	static inline Int2 size{ 1920, 1080 };

private:
	Timer m_timer;
};
