#pragma once
#include "Button.h"
#include "Window.h"

namespace
{
	constexpr std::wstring_view BUTTON_PREFIX{ L"Button:" };
}

template<class T>
requires std::is_base_of_v<IWindow, T>
class WindowBuilder
{
public:
	WindowBuilder() :
		m_window{ new T{} }
	{
	}

	~WindowBuilder() = default;

	WindowBuilder& Path(std::wstring_view path)
	{
		auto prop{ Resource::Get(path) };
		return *this;
	}

	WindowBuilder& Size(const INT2& size)
	{
		m_window->SetSize(size);
		return *this;
	}

	WindowBuilder& Position(const INT2& position, Pivot pivot = Pivot::LeftTop)
	{
		m_window->SetPosition(position, pivot);
		return *this;
	}

	std::shared_ptr<T> Build()
	{
		return m_window;
	}

private:
	std::shared_ptr<T> m_window;
};
