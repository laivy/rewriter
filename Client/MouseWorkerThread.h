#pragma once

template<class T>
class TSingleton;

class MouseWorkerThread : public TSingleton<MouseWorkerThread>
{
public:
	MouseWorkerThread();
	~MouseWorkerThread();

	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	void Run();

private:
	std::thread m_thread;
	BOOL m_isRunning;
	BOOL m_doProcess;

	HWND m_hWnd;
	UINT m_message;
	WPARAM m_wParam;
	LPARAM m_lParam;
};