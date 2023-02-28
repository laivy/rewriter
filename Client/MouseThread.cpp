#include "Stdafx.h"
#include "MouseThread.h"
#include "WndManager.h"

MouseThread::MouseThread() : m_isRunning{ TRUE }, m_doProcess{ FALSE }
{
	m_thread = std::thread{ &MouseThread::Run, this };
}

MouseThread::~MouseThread()
{
	m_isRunning = FALSE;
	m_thread.join();
}

void MouseThread::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_message = message;
	m_wParam = wParam;
	m_lParam = lParam;
	m_doProcess = TRUE;
}

void MouseThread::Run()
{
	while (m_isRunning)
	{
		if (!m_doProcess)
			continue;

		if (WndManager::IsInstanced())
			WndManager::GetInstance()->OnMouseEvent(m_hWnd, m_message, m_wParam, m_lParam);

		m_doProcess = FALSE;
	}
}