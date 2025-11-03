#pragma once
#include "Common/Timer.h"

class App : public TSingleton<App>
{
public:
	App();
	~App();

	void Run();

	Int2 GetCursorPosition() const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void InitWindow();
	void InitApp();

	void Update();
	void Render();

private:
	static constexpr auto WindowName{ L"Editor" };

	HWND m_hWnd;
	Int2 m_windowSize;
	Timer m_timer;
};
