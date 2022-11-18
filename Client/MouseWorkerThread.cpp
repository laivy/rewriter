#include "Stdafx.h"
#include "MouseWorkerThread.h"
#include "WndManager.h"

MouseWorkerThread::MouseWorkerThread() : m_isRunning{ TRUE }, m_doProcess{ FALSE }
{
	m_thread = std::thread{ &MouseWorkerThread::Run, this };
}

MouseWorkerThread::~MouseWorkerThread()
{
	m_isRunning = FALSE;
	m_thread.join();
}

void MouseWorkerThread::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_message = message;
	m_wParam = wParam;
	m_lParam = lParam;
	m_doProcess = TRUE;
}

void MouseWorkerThread::Run()
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