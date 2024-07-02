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
	void InitImgui();
	void InitApp();

	void Update();
	void Render();

public:
	static constexpr auto WINDOW_TITLE_NAME{ L"LOGIN SERVER" };
	static inline auto OnResize{ std::make_shared<Event<int, int >>() };
	static inline auto OnPacket{ std::make_shared<Event<size_t, std::shared_ptr<Packet>>>() };
	static inline HWND hWnd{ NULL };
	static inline INT2 size{ 600, 400 };

private:
	Timer m_timer;
};