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
	static inline auto OnResize{ Delegate<int, int>() };
	static inline auto OnPropertyAdd{ Delegate<const std::shared_ptr<Resource::Property>&>() };
	static inline auto OnPropertyDelete{ Delegate<const std::shared_ptr<Resource::Property>&>() };
	static inline auto OnPropertyModified{ Delegate<const std::shared_ptr<Resource::Property>&>() };
	static inline auto OnPropertySelected{ Delegate<const std::shared_ptr<Resource::Property>&>() };

	static constexpr auto WINDOW_TITLE_NAME{ L"ENGINE" };
	static inline HWND hWnd{ NULL };
	static inline Int2 size{ 1920, 1080 };

private:
	bool m_isActive;
	Timer m_timer;
};
