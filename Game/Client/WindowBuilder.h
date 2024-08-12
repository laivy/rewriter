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
	void Path(const std::shared_ptr<Resource::Property>& prop, const std::wstring& basePath)
	{
		auto ToPivot = [](std::wstring_view pivot)
			{
				if (pivot == L"Center")
					return Pivot::CENTER;
				return Pivot::LEFTTOP;
			};

		for (const auto& child : prop->GetChildren())
		{
			constexpr auto CONTROL_TYPE_BUTTON{ L"Button: " };

			// 버튼
			auto name{ child->GetName() };
			if (name.starts_with(CONTROL_TYPE_BUTTON))
			{
				auto button{ std::make_shared<Button>(m_window.get()) };

				// 이름
				std::wstring buttonName{ basePath + name.substr(std::char_traits<wchar_t>::length(CONTROL_TYPE_BUTTON)) };
				button->SetName(buttonName);

				// 크기
				auto defaultImage{ prop->GetImage(L"Default") };
				button->SetSize(defaultImage->GetSize());

				// 위치
				button->SetPosition(prop->GetInt2(L"Position"), ToPivot(prop->GetString(L"Pivot")));

				static_cast<IWindow*>(m_window.get())->Register(button);
			}

			// 재귀
			Path(prop, name + L"/");
		}
	}

private:
	std::shared_ptr<T> m_window;
	std::shared_ptr<Resource::Property> m_property;
};