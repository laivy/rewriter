#pragma once

class Timer;

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
	static inline Event<UINT, WPARAM, LPARAM> OnKeyboardEvent;
	static inline Event<UINT, int, int> OnMouseEvent;
	static inline Event<int, int> OnResize;
	static inline HINSTANCE hInstance{ NULL };
	static inline HWND hWnd{ NULL };
	static inline INT2 size{ 1920, 1080 };

private:
	bool m_isActive;
	std::unique_ptr<Timer> m_timer;
};