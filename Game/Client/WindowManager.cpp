#include "Stdafx.h"
#include "App.h"
#include "Modal.h"
#include "Renderer.h"
#include "Window.h"
#include "WindowManager.h"

#include "DebugWindow.h"

WindowManager::WindowManager() :
	m_focusWindow{ nullptr }
{
	App::OnKeyboardEvent.Register(this, std::bind_front(&WindowManager::OnKeyboardEvent, this));
	App::OnMouseEvent.Register(this, std::bind_front(&WindowManager::OnMouseEvent, this));

#ifdef _DEBUG
	for (size_t i = 0; i < 1; ++i)
	{
		auto window{ std::make_unique<DebugWindow>() };
		if (i == 0)
			window->SetPosition({ App::size.x / 2, App::size.y / 2 }, Pivot::CENTER);
		Register(std::move(window));
	}
#endif // _DEBUG
}

void WindowManager::Update(float deltaTime)
{
	std::erase_if(m_modals, [this](const auto& modal) { return !modal->IsValid(); });
	std::erase_if(m_windows, [this](const auto& window) { return !window->IsValid(); });
	if (!m_modals.empty())
		m_focusWindow = m_modals.back().get();
	else if (!m_windows.empty())
		m_focusWindow = m_windows.back().get();
	else
		m_focusWindow = nullptr;

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

void WindowManager::Register(std::unique_ptr<IModal> modal)
{
	m_focusWindow = modal.get();
	m_modals.push_back(std::move(modal));
}

void WindowManager::Register(std::unique_ptr<IWindow> window)
{
	if (m_modals.empty())
		m_focusWindow = window.get();
	m_windows.push_back(std::move(window));
}

void WindowManager::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_focusWindow)
		m_focusWindow->OnKeyboardEvent(message, wParam, lParam);
}

void WindowManager::OnMouseEvent(UINT message, int x, int y)
{
	if (!m_modals.empty())
	{
		auto pos{ m_modals.back()->GetPosition() };
		m_modals.back()->OnMouseEvent(message, x - pos.x, y - pos.y);
		return;
	}

	if (m_windows.empty())
		return;

	// 최상위 윈도우 갱신
	do
	{
		if (message != WM_LBUTTONDOWN)
			break;

		auto rit = std::ranges::find_if(std::views::reverse(m_windows), [x, y](const auto& window) { return window->IsContain({ x, y }); });
		if (rit == m_windows.rend())
		{
			m_focusWindow = nullptr;
			break;
		}

		auto it{ --rit.base() };
		std::rotate(it, it + 1, m_windows.end());

		m_focusWindow = m_windows.back().get();
		m_focusWindow->SetFocus(true);
	} while (false);

	for (const auto& window : m_windows)
	{
		if (!window->IsContain({ x, y }))
			continue;

		auto pos{ window->GetPosition() };
		window->OnMouseEvent(message, x - pos.x, y - pos.y);
	}
}
