#include "Stdafx.h"
#include "KeyWorkerThread.h"
#include "WndManager.h"

KeyboardWorkerThread::KeyboardWorkerThread() : m_isRunning{ TRUE }, m_doProcess{ FALSE }
{
	m_thread = std::thread{ &KeyboardWorkerThread::Run, this };
}

KeyboardWorkerThread::~KeyboardWorkerThread()
{
	m_isRunning = FALSE;
	m_thread.join();
}

void KeyboardWorkerThread::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_message = message;
	m_wParam = wParam;
	m_lParam = lParam;
	m_doProcess = TRUE;
}

void KeyboardWorkerThread::Run()
{
	while (m_isRunning)
	{
		if (!m_doProcess)
			continue;

		if (WndManager::IsInstanced())
			WndManager::GetInstance()->OnKeyboardEvent(m_hWnd, m_message, m_wParam, m_lParam);

		m_doProcess = FALSE;
	}
}