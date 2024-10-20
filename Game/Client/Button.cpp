#include "Stdafx.h"
#include "App.h"
#include "Button.h"
#include "Window.h"

Button::Button(IWindow* owner) :
	IControl{ owner },
	m_state{ State::Normal }
{
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
		OnButtonClick.Notify();
		m_state = State::Hover;
		break;
	}
	default:
		break;
	}
}

void Button::Render() const
{
	auto draw = [](const Visual& visual, const FLOAT2& position)
		{
			if (std::holds_alternative<std::shared_ptr<Resource::Sprite>>(visual))
			{
				auto sprite{ std::get<std::shared_ptr<Resource::Sprite>>(visual) };
				Graphics::D2D::DrawSprite(sprite, FLOAT2{ position });
			}
			else
			{
				const auto& [size, radius, color] { std::get<std::tuple<FLOAT2, FLOAT2, int32_t>>(visual) };
				RECTF rect{ 0.0f, 0.0f, size.x, size.y };
				Graphics::D2D::DrawRoundRect(rect.Offset(position), radius, color);
			}
		};

	switch (m_state)
	{
	case State::Normal:
	{
		draw(m_normal, m_position);
		break;
	}
	case State::Hover:
	{
		draw(m_hover, m_position);
		break;
	}
	case State::Active:
	{
		draw(m_active, m_position);
		break;
	}
	case State::Disable:
	{
		draw(m_disable, m_position);
		break;
	}
	default:
		break;
	}
}

void Button::SetVisuals(const Visual& normal, const Visual& hover, const Visual& active, const Visual& disable)
{
	m_normal = normal;
	m_hover = hover;
	m_active = active;
	m_disable = disable;

	// 버튼 크기는 "normal" 을 기준으로 설정
	if (std::holds_alternative<std::shared_ptr<Resource::Sprite>>(m_normal))
	{
		auto sprite{ std::get<std::shared_ptr<Resource::Sprite>>(m_normal) };
		SetSize(sprite->GetSize());
	}
	else
	{
		const auto& [size, _, __] {std::get<std::tuple<FLOAT2, FLOAT2, int32_t>>(m_normal)};
		SetSize(size);
	}
}
