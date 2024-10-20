#pragma once
#include "Button.h"
#include "Window.h"

namespace
{
	constexpr std::wstring_view NINE_PATCH{ L"NinePatch" };
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
		Path(Resource::Get(path));
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
	void Path(const std::shared_ptr<Resource::Property>& root, std::wstring_view basePath = L"")
	{
		for (const auto& [name, prop] : *root)
		{
			if (name.starts_with(NINE_PATCH))
				m_window->GetLayer(0);
			if (name.starts_with(BUTTON_PREFIX))
				RegisterButton(prop, basePath);
			Path(prop, std::format(L"{}/{}", basePath, name));
		}
	}

	void RegisterButton(const std::shared_ptr<Resource::Property>& prop, std::wstring_view basePath)
	{
		/*
		- Z(Int)
			- 그려질 레이어의 z값
		- Position(INT2)
			- 위치
		- Normal, Hover, Active, Disable
			- 각 상태일 때 보여질 이미지. 아래 1, 2번 중 한 가지여야 함
			1. Sprite
				- 해당 스프라이트를 그림
			2. Folder
				- Size(INT2) : 가로, 세로 길이
				- Radius(INT2) : 모서리의 둥근 정도
				- Color(Int) : RGB(0xRRGGBB)
		*/

		auto name{ prop->GetName().substr(BUTTON_PREFIX.size()) };
		auto z{ prop->GetInt(L"Z") };
		auto position{ prop->GetInt2(L"Position") };

		std::array<Button::Visual, 4> visuals{};
		size_t index{ 0 };
		for (const auto& key : { L"Normal", L"Hover", L"Active", L"Disable" })
		{
			auto visual{ prop->Get(key) };
			switch (visual->GetType())
			{
			case Resource::Property::Type::Folder:
			{
				FLOAT2 size{ visual->GetInt2(L"Size") };
				FLOAT2 radius{ visual->GetInt2(L"Radius") };
				int32_t color{ visual->GetInt(L"Color") };
				visuals[index] = std::make_tuple(size, radius, color);
				break;
			}
			case Resource::Property::Type::Sprite:
			{
				visuals[index] = visual->GetSprite();
				break;
			}
			default:
				break;
			}
			++index;
		}

		auto button{ std::make_shared<Button>(m_window.get()) };
		button->SetName(basePath.empty() ? name : std::format(L"{}/{}", basePath, name));
		button->SetZ(z);
		button->SetPosition(position);
		button->SetVisuals(visuals[0], visuals[1], visuals[2], visuals[3]);
		m_window->Register(button);
	}

private:
	std::shared_ptr<T> m_window;
};
