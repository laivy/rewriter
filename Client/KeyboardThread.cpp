#include "Stdafx.h"
#include "KeyboardThread.h"
#include "WndManager.h"

KeyboardThread::KeyboardThread() : m_isRunning{ TRUE }, m_doProcess{ FALSE }
{
	m_thread = std::thread{ &KeyboardThread::Run, this };
}

KeyboardThread::~KeyboardThread()
{
	m_isRunning = FALSE;
	m_thread.join();
}

void KeyboardThread::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_message = message;
	m_wParam = wParam;
	m_lParam = lParam;
	m_doProcess = TRUE;
}

void KeyboardThread::Run()
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