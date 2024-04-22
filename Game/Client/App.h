#pragma once

class Timer;

class App : public TSingleton<App>
{
public:
	App();
	~App() = default;

	void OnCreate();

	void Run();

	HWND GetHwnd() const;
	INT2 GetWindowSize() const;
	INT2 GetCursorPosition() const;

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void OnDestroy();

	HRESULT InitWindow();

	void Update();
	void Render();

private:
	bool m_isActive;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	INT2 m_size;
	std::unique_ptr<Timer> m_timer;

public:
	static inline Event<UINT, WPARAM, LPARAM> OnKeyboardEvent;
	static inline Event<UINT, int, int> OnMouseEvent;
	static inline Event<int, int> OnResize;
};