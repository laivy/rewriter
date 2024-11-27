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
	static constexpr auto WINDOW_TITLE_NAME{ L"CENTER SERVER" };
	static inline Delegate<int, int> OnResize;
	static inline HWND hWnd{ NULL };
	static inline Int2 size{ 600, 400 };

private:
	Timer m_timer;
};
