#include "Stdafx.h"
#include "App.h"
#include "Modal.h"
#include "Renderer.h"
#include "Window.h"
#include "WindowManager.h"

WindowManager::WindowManager()
{
	App::OnKeyboardEvent.Register(this, std::bind_front(&WindowManager::OnKeyboardEvent, this));
	App::OnMouseEvent.Register(this, std::bind_front(&WindowManager::OnMouseEvent, this));
}

void WindowManager::Update(float deltaTime)
{
	std::erase_if(m_modals, [this](const auto& modal) { return !modal->IsValid(); });
	std::erase_if(m_windows, [this](const auto& window) { return !window->IsValid(); });

	if (!m_modals.empty())
		m_focusWindow = m_modals.back();
	else if (!m_windows.empty())
		m_focusWindow = m_windows.back();
	else
		m_focusWindow.reset();

	for (const auto& modal : m_modals)
		modal->Update(deltaTime);
	for (const auto& window : m_windows)
		window->Update(deltaTime);
}

void WindowManager::Render() const
{
	for (const auto& window : m_windows)
	{
		auto pos{ window->GetPosition() };
		Renderer::ctx->SetTransform(D2D1::Matrix3x2F::Translation(static_cast<float>(pos.x), static_cast<float>(pos.y)));
		window->Render();
	}
	Renderer::ctx->SetTransform(D2D1::Matrix3x2F::Identity());

	for (const auto& modal : m_modals)
	{
		auto pos{ modal->GetPosition() };
		Renderer::ctx->SetTransform(D2D1::Matrix3x2F::Translation(static_cast<float>(pos.x), static_cast<float>(pos.y)));
		modal->Render();
	}
	Renderer::ctx->SetTransform(D2D1::Matrix3x2F::Identity());
}

void WindowManager::Register(const std::shared_ptr<IModal>& modal)
{
	m_focusWindow = modal;
	m_modals.push_back(modal);
}

void WindowManager::Register(const std::shared_ptr<IWindow>& window)
{
	if (m_modals.empty())
		m_focusWindow = window;
	m_windows.push_back(window);
}

void WindowManager::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (auto focusWindow{ m_focusWindow.lock() })
		focusWindow->OnKeyboardEvent(message, wParam, lParam);
}

void WindowManager::OnMouseEvent(UINT message, int x, int y)
{
	if (message == WM_MOUSEMOVE)
		UpdateMouseOverWindow(x, y);

	// 모달이 있으면 해당 모달에게만 이벤트 전달
	if (!m_modals.empty())
	{
		auto& modal{ m_modals.back() };
		auto pos{ modal->GetPosition() };
		modal->OnMouseEvent(message, x - pos.x, y - pos.y);
		return;
	}

	if (message == WM_LBUTTONDOWN)
		UpdateFocusWindow(x, y);

	// 이벤트 전달
	for (const auto& window : m_windows)
	{
		if (!window->IsContain({ x, y }))
			continue;

		auto pos{ window->GetPosition() };
		window->OnMouseEvent(message, x - pos.x, y - pos.y);
	}
}

void WindowManager::UpdateMouseOverWindow(int x, int y)
{
	if (!m_modals.empty())
	{
		auto rit{ std::ranges::find_if(std::views::reverse(m_modals), [x, y](const auto& modal) { return modal->IsContain({ x, y }); }) };
		if (rit == m_modals.rend())
		{
			if (auto window{ m_mouseOverWindow.lock() })
				window->OnMouseLeave(x, y);
			m_mouseOverWindow.reset();
			return;
		}

		if (auto window{ m_mouseOverWindow.lock() }; window != *rit)
		{
			if (window)
				window->OnMouseLeave(x, y);
			m_mouseOverWindow = *rit;
			(*rit)->OnMouseEnter(x, y);
		}
		return;
	}

	auto rit{ std::ranges::find_if(std::views::reverse(m_windows), [x, y](const auto& window) { return window->IsContain({ x, y }); }) };
	if (rit == m_windows.rend())
	{
		if (auto window{ m_mouseOverWindow.lock() })
			window->OnMouseLeave(x, y);
		m_mouseOverWindow.reset();
		return;
	}

	if (auto window{ m_mouseOverWindow.lock() }; window != *rit)
	{
		if (window)
			window->OnMouseLeave(x, y);
		m_mouseOverWindow = *rit;
		(*rit)->OnMouseEnter(x, y);
	}
}

void WindowManager::UpdateFocusWindow(int x, int y)
{
	auto rit{ std::ranges::find_if(std::views::reverse(m_windows), [x, y](const auto& window) { return window->IsContain({ x, y }); }) };
	if (rit == m_windows.rend())
	{
		m_focusWindow.reset();
		return;
	}

	auto it{ --rit.base() };
	std::rotate(it, it + 1, m_windows.end());
	m_windows.back()->SetFocus(true);
	m_focusWindow = m_windows.back();
}
