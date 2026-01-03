#pragma once
#include <Common/Timer.h>

class App : public Singleton<App>
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
	HWND m_hWnd;
	Int2 m_windowSize;
	Timer m_timer;
};
