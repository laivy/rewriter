#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Window.h"

Button::Button(IWindow* owner, const std::shared_ptr<Resource::Property>& prop) :
	IControl{ owner },
	m_state{ State::Normal }
{
	Build(prop);
}

void Button::OnMouseEnter(int x, int y)
{
	m_state = State::Hover;
}

void Button::OnMouseLeave(int x, int y)
{
	m_state = State::Normal;
}

void Button::OnMouseEvent(UINT message, int x, int y)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_state = State::Active;
		break;
	}
	case WM_LBUTTONUP:
	{
		if (m_state == State::Active)
		{
			OnButtonClick.Notify();
			m_state = State::Hover;
		}
		break;
	}
	default:
		break;
	}
}

void Button::Render() const
{
	const auto& visual{ m_visuals[static_cast<size_t>(m_state)] };
	if (std::holds_alternative<std::shared_ptr<Resource::Sprite>>(visual))
	{
		auto sprite{ std::get<std::shared_ptr<Resource::Sprite>>(visual) };
		Graphics::D2D::DrawSprite(sprite, Float2{ m_position });
	}
	else
	{
		const auto& [size, radius, color] { std::get<1>(visual) };
		RectF rect{ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) };
		Graphics::D2D::DrawRoundRect(rect.Offset(m_position), radius, color);
	}
}

void Button::Build(const std::shared_ptr<Resource::Property>& prop)
{
	/*
	- Z(Int)
		- 그려질 레이어의 z값
	- Position(Int2)
		- 위치
	- Normal, Hover, Active, Disable
		- 각 상태일 때 보여질 이미지. 아래 1, 2번 중 한 가지여야 함
		1. Sprite
			- 해당 스프라이트를 그림
		2. Folder
			- Size(Int2) : 가로, 세로 길이
			- Radius(Int2) : 모서리의 둥근 정도
			- Color(Int) : RGB(0xRRGGBB)
	*/

	if (!prop)
		return;

	SetZ(prop->GetInt(L"Z"));
	SetPosition(prop->GetInt2(L"Position"));

	std::array visuals{
		std::pair{ L"Normal", Visual{} },
		std::pair{ L"Hover", Visual{} },
		std::pair{ L"Active", Visual{} },
		std::pair{ L"Disable", Visual{} }
	};

	for (auto& [key, visual] : visuals)
	{
		auto p{ prop->Get(key) };
		switch (p->GetType())
		{
		case Resource::Property::Type::Folder:
		{
			Int2 size{ p->GetInt2(L"Size") };
			Float2 radius{ p->GetInt2(L"Radius") };
			Graphics::D2D::Color color{ static_cast<uint32_t>(p->GetInt(L"Color")) };
			visual = std::make_tuple(size, radius, color);
			break;
		}
		case Resource::Property::Type::Sprite:
		{
			visual = p->GetSprite();
			break;
		}
		default:
			assert(false && "INVALID TYPE");
			break;
		}
	}

	std::ranges::copy(visuals | std::views::elements<1>, m_visuals.begin());

	// 크기는 "normal" 을 기준으로 설정
	if (std::holds_alternative<std::shared_ptr<Resource::Sprite>>(m_visuals[0]))
	{
		auto sprite{ std::get<std::shared_ptr<Resource::Sprite>>(m_visuals[0]) };
		SetSize(sprite->GetSize());
	}
	else
	{
		const auto& [size, _, __] { std::get<1>(m_visuals[0]) };
		SetSize(size);
	}
}
