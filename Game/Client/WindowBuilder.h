#pragma once
#include "Button.h"
#include "Window.h"

template<class T>
requires std::is_base_of_v<IWindow, T>
class WindowBuilder
{
public:
	WindowBuilder() : m_window{ new T{} }
	{
	}

	~WindowBuilder() = default;

	WindowBuilder& Path(std::wstring_view path)
	{
		m_property = Resource::Get(path);
		Path(m_property, L"");
		return *this;
	}

	WindowBuilder& Size(const INT2& size)
	{
		m_window->SetSize(size);
		return *this;
	}

	WindowBuilder& Position(const INT2& position, Pivot pivot = Pivot::LEFTTOP)
	{
		m_window->SetPosition(position, pivot);
		return *this;
	}

	std::shared_ptr<T> Build()
	{
		return m_window;
	}

private:
	void Path(const std::shared_ptr<Resource::Property>& prop, const std::wstring& basePath);

private:
	std::shared_ptr<T> m_window;
	std::shared_ptr<Resource::Property> m_property;
};
