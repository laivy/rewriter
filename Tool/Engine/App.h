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
	void InitImGui();
	void InitApp();

	void Update();
	void Render();

	void RenderImGuiMainDockSpace();

public:
	static constexpr auto WINDOW_TITLE_NAME{ L"ENGINE" };
	static inline auto OnResize{ std::make_shared<Event<int, int>>() };
	static inline HWND hWnd{ NULL };
	static inline INT2 size{ 1920, 1080 };
	
private:
	bool m_isActive;
	std::unique_ptr<Timer> m_timer;
};