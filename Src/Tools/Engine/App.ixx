module;

// Windows
#include <Windows.h>

export module rewriter.tool.engine.app;

import rewriter.common.singleton;
import rewriter.common.timer;
import rewriter.common.type;

export class App : public TSingleton<App>
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
	static constexpr auto WINDOW_TITLE_NAME{ L"Engine" };

	HWND m_hWnd;
	Int2 m_windowSize;
	Timer m_timer;
};
