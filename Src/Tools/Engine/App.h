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

private:
	static constexpr auto WINDOW_TITLE_NAME{ L"Engine" };

public:
	static inline HWND hWnd{ NULL };
	static inline Int2 size{ 1920, 1080 };

private:
	bool m_isActive;
	Timer m_timer;
};
