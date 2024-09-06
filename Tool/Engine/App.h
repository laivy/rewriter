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

	void RenderImGuiMainDockSpace();

public:
	static inline auto OnResize{ Delegate<int, int>() };
	static inline auto OnPropertyAdd{ Delegate<std::shared_ptr<Resource::Property>>() };
	static inline auto OnPropertyDelete{ Delegate<std::shared_ptr<Resource::Property>>() };
	static inline auto OnPropertySelect{ Delegate<std::shared_ptr<Resource::Property>>() };

	static constexpr auto WINDOW_TITLE_NAME{ L"ENGINE" };
	static inline HWND hWnd{ NULL };
	static inline INT2 size{ 1920, 1080 };	

private:
	bool m_isActive;
	Timer m_timer;
};
