#include "Stdafx.h"
#include "App.h"
#include "TextBox.h"
#include "Window.h"

TextBox::TextBox(IWindow* owner, const std::shared_ptr<Resource::Property>& prop) :
	IControl{ owner },
	m_visuals{},
	m_font{ L"", 16.0f },
	m_metrics{},
	m_isMultiLine{},
	m_isPassword{},
	m_onCompositing{},
	m_isCaretVisible{ true },
	m_caretTimer{},
	m_caretPosition{},
	m_scrollOffset{}
{
	Build(prop);
}

void TextBox::Update(float deltaTime)
{
	if (IsFocus())
	{
		m_caretTimer += deltaTime;
		if (m_caretTimer > 1.0f)
		{
			m_caretTimer -= 1.0f;
			m_isCaretVisible = !m_isCaretVisible;
		}
	}
}

void TextBox::Render() const
{
	RenderBackground();
	RenderText();
	RenderCaret();
}

void TextBox::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_BACK:
		{
			if (m_caretPosition == 0)
				break;
			MoveCaret(-1);
			EraseCharacter();
			break;
		}
		case VK_END:
		{
			MoveCaret(static_cast<int>(m_text.size()));
			break;
		}
		case VK_HOME:
		{
			MoveCaret(-static_cast<int>(m_text.size()));
			break;
		}
		case VK_LEFT:
		{
			MoveCaret(-1);
			break;
		}
		case VK_RIGHT:
		{
			MoveCaret(1);
			break;
		}
		case VK_DELETE:
		{
			EraseCharacter();
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_CHAR:
	{
		if (wParam == VK_BACK)
			break;

		if (wParam == VK_RETURN && !m_isMultiLine)
			break;

		InsertCharacter(static_cast<wchar_t>(wParam));
		MoveCaret(1);
		break;
	}
	case WM_IME_COMPOSITION:
	{
		if (lParam & GCS_COMPSTR)
		{
			HIMC hImc{ ::ImmGetContext(App::hWnd) };
			int length{ ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0) };

			// 조합중이라면 글자 교체
			if (m_onCompositing && !m_text.empty())
			{
				MoveCaret(-1);
				EraseCharacter();
			}

			// 백스페이스로 조합 중인 글자를 다 지우면 길이가 0이될 수 있음
			m_onCompositing = length > 0;
			if (m_onCompositing)
			{
				InsertCharacter(static_cast<wchar_t>(wParam));
				MoveCaret(1);
			}

			::ImmReleaseContext(App::hWnd, hImc);
		}
		if (lParam & GCS_RESULTSTR)
		{
			if (m_onCompositing)
			{
				MoveCaret(-1);
				EraseCharacter();
			}
			InsertCharacter(static_cast<wchar_t>(wParam));
			MoveCaret(1);
			m_onCompositing = false;
		}
		break;
	}
	default:
		break;
	}
}

void TextBox::SetFocus(bool focus)
{
	IControl::SetFocus(focus);
	if (focus)
		MoveCaret(static_cast<int>(m_text.size()));
}

bool TextBox::IsFocus() const
{
	if (!IControl::IsFocus())
		return false;
	if (!m_owner->IsFocus())
		return false;
	return true;
}

std::wstring TextBox::GetText() const
{
	return m_text;
}

void TextBox::Build(const std::shared_ptr<Resource::Property>& prop)
{
	/*
	- Z(Int)
		- 그려질 레이어의 z값
	- Position(Int2)
		- 위치
	- FontName(String)
		- 폰트 이름
	- FontSize(Float)
		- 폰트 크기
	- MultiLine(Int)
		- 1이면 여러줄 입력 가능. 그 외는 한 줄만 입력 가능
	- Password(Int)
		- 1이면 텍스트를 '*' 로 표시
	- Normal, Focus
		- 각 상태일 때 보여질 이미지. 아래 1, 2번 중 한 가지여야 함
		1. Sprite
			- 해당 스프라이트를 그림
		2. Folder
			- Size(Int2) : 가로, 세로 길이
			- BackgroundColor(Int) : 배경 색깔 RGB(0xRRGGBB)
			- OutlineColor(Int) : 테두리 색깔 RGB(0xRRGGBB)
	*/

	if (!prop)
		return;

	SetZ(prop->GetInt(L"Z"));
	SetPosition(prop->GetInt2(L"Position"));
	m_font.name = prop->GetString(L"FontName");
	m_font.size = prop->GetFloat(L"FontSize");
	m_isMultiLine = prop->GetInt(L"MultiLine") == 1;
	m_isPassword = prop->GetInt(L"Password") == 1;

	std::array visuals{
		std::pair{ L"Normal", Visual{} },
		std::pair{ L"Focus", Visual{} }
	};

	for (auto& [key, visual] : visuals)
	{
		auto p{ prop->Get(key) };
		switch (p->GetType())
		{
		case Resource::Property::Type::Folder:
		{
			INT2 size{ p->GetInt2(L"Size") };
			int32_t backgroundColor{ p->GetInt(L"BackgroundColor") };
			int32_t outlineColor{ p->GetInt(L"OutlineColor") };
			visual = std::make_tuple(size, backgroundColor, outlineColor);
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

void TextBox::RenderBackground() const
{
	const auto& visual{ IsFocus() ? m_visuals[1] : m_visuals[0] };
	if (visual.index() == 0)
	{

	}
	else
	{
		auto [size, backgroundColor, outlineColor] { std::get<1>(visual) };

		RECTF rect{ 0, 0, static_cast<float>(size.x), static_cast<float>(size.y) };
		rect.Offset(m_position);

		RECTF outlineRect{ rect };
		outlineRect.Offset({ 0, 3 });

		Graphics::D2D::DrawRoundRect(outlineRect, { 7.0f, 7.0f }, outlineColor);
		Graphics::D2D::DrawRoundRect(rect, { 5.0f, 5.0f }, backgroundColor);
	}
}

void TextBox::RenderText() const
{
	Graphics::D2D::PushClipRect(
		RECTF{
			PADDING_LEFT,
			PADDING_TOP,
			static_cast<float>(m_size.x - PADDING_RIGHT),
			static_cast<float>(m_size.y - PADDING_BOTTOM)
		}.Offset(m_position)
	);

	std::wstring text;
	if (m_isPassword)
		text.assign(m_text.size(), L'*');
	else
		text = m_text;
	Graphics::D2D::DrawText(text, m_font, Graphics::D2D::Color::Black, m_position + INT2{ PADDING_LEFT, PADDING_TOP } + m_scrollOffset, Pivot::LeftTop);
	Graphics::D2D::PopClipRect();
}

void TextBox::RenderCaret() const
{
	if (!IsFocus() || !m_isCaretVisible)
		return;

	constexpr auto CARET_WIDTH{ 2 };
	RECTI caret{ 0, 0, CARET_WIDTH, static_cast<int>(m_font.size) };
	caret.Offset(m_position);
	caret.Offset(INT2{ m_metrics.left, m_metrics.top });
	caret.Offset(INT2{ m_scrollOffset.x, m_scrollOffset.y });
	caret.Offset(INT2{ PADDING_LEFT, static_cast<int>(m_metrics.height - m_font.size) });
	Graphics::D2D::DrawRect(caret, Graphics::D2D::Color::Black);
}

void TextBox::SetText(std::wstring_view text)
{
	m_text = text;

	std::wstring temp;
	if (m_isPassword)
		temp.assign(m_caretPosition, L'*');
	else
		temp = m_text.substr(0, m_caretPosition);
	m_metrics = Graphics::D2D::GetTextMetrics(temp, m_font);

	m_isCaretVisible = true;
	m_caretTimer = 0.0f;
}

void TextBox::MoveCaret(int distance)
{
	m_caretPosition = std::clamp(m_caretPosition + distance, 0, static_cast<int>(m_text.size()));

	std::wstring temp;
	if (m_isPassword)
		temp.assign(m_caretPosition, L'*');
	else
		temp = m_text.substr(0, m_caretPosition);
	m_metrics = Graphics::D2D::GetTextMetrics(temp, m_font);

	m_isCaretVisible = true;
	m_caretTimer = 0.0f;

	UpdateScrollOffset();
}

void TextBox::UpdateScrollOffset()
{
	int textWidth{ static_cast<int>(m_metrics.left) };
	int textBoxWidth{ m_size.x - (PADDING_LEFT + PADDING_RIGHT) };
	int textBoxHeight{ m_size.y - (PADDING_TOP + PADDING_BOTTOM) };

	// 우측 스크롤
	if (textWidth + m_scrollOffset.x >= textBoxWidth)
		m_scrollOffset.x = textBoxWidth - textWidth;

	// 좌측 스크롤
	else if (textWidth + m_scrollOffset.x < 0)
		m_scrollOffset.x = -textWidth;

	// 하단 스크롤
	if (m_metrics.top + m_metrics.height + m_scrollOffset.y > textBoxHeight)
		m_scrollOffset.y = textBoxHeight - static_cast<int>(m_metrics.top + m_metrics.height);

	// 상단 스크롤
	else if (m_metrics.top + m_scrollOffset.y < 0)
		m_scrollOffset.y = static_cast<int>(-m_metrics.top) + PADDING_TOP;
}

void TextBox::InsertCharacter(wchar_t character)
{
	m_text.insert(m_caretPosition, 1, character);
	SetText(m_text);
}

void TextBox::EraseCharacter()
{
	if (m_text.empty())
		return;

	m_text.erase(m_caretPosition, 1);
	SetText(m_text);
}
