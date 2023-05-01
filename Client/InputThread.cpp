#include "Stdafx.h"
#include "InputThread.h"
#include "SceneManager.h"

InputThread::InputThread() : m_isRunning{ TRUE }, m_doProcess{ FALSE }
{
	m_thread = std::thread{ &InputThread::Run, this };
}

InputThread::~InputThread()
{
	m_isRunning = FALSE;
	m_thread.join();
}

void InputThread::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
	m_message = message;
	m_wParam = wParam;
	m_lParam = lParam;
	m_doProcess = TRUE;
}

void InputThread::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}

void InputThread::Run()
{
	while (m_isRunning)
	{
		if (!m_doProcess)
			continue;

		if (SceneManager::IsInstanced())
			SceneManager::GetInstance()->OnMouseEvent(m_hWnd, m_message, m_wParam, m_lParam);

		m_doProcess = FALSE;
	}
}