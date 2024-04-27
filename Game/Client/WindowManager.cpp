#include "Stdafx.h"
#include "App.h"
#include "Modal.h"
#include "Window.h"
#include "WindowManager.h"

WindowManager::WindowManager()
{
	App::OnKeyboardEvent.Register(this, std::bind_front(&WindowManager::OnKeyboardEvent, this));
	App::OnMouseEvent.Register(this, std::bind_front(&WindowManager::OnMouseEvent, this));
}

void WindowManager::Update(float deltaTime)
{
	for (const auto& modal : m_modals)
		modal->Update(deltaTime);
	for (const auto& window : m_windows)
		window->Update(deltaTime);
}

void WindowManager::Render() const
{
	for (const auto& modal : m_modals)
		modal->Render();
	for (const auto& window : m_windows)
		window->Render();
}

void WindowManager::Register(std::unique_ptr<IModal> modal)
{
	m_modals.push_front(std::move(modal));
}

void WindowManager::Register(std::unique_ptr<IWindow> window)
{
	m_windows.push_back(std::move(window));
}

void WindowManager::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_modals.empty())
	{
		m_modals.front()->OnKeyboardEvent(message, wParam, lParam);
		return;
	}
	for (const auto& window : m_windows)
		window->OnKeyboardEvent(message, wParam, lParam);
}

void WindowManager::OnMouseEvent(UINT message, int x, int y)
{
	if (!m_modals.empty())
	{
		m_modals.front()->OnMouseEvent(message, x, y);
		return;
	}
	for (const auto& window : m_windows)
		window->OnMouseEvent(message, x, y);
}
