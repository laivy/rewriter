#include "Stdafx.h"
#include "App.h"
#include "TextBox.h"
#include "Window.h"

TextBox::TextBox(IWindow* owner) :
	IControl{ owner },
	m_isCompositing{ false },
	m_caret{ 0 },
	m_caretTimer{ 0.0f },
	m_drawCaret{ true },
	m_offset{},
	m_isMultiLine{ false },
	m_hasVerticalScroll{ false },
	m_hasHorizontalScroll{ false }
{
	//m_textFormat = Renderer2D::CreateTextFormat(L"", 16);
}

void TextBox::OnMouseEvent(UINT message, int x, int y)
{
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
			MoveCaret(-1);
			EraseCharacter();
			break;
		case VK_END:
			MoveCaret(static_cast<int>(m_text.size()));
			break;
		case VK_HOME:
			MoveCaret(-static_cast<int>(m_text.size()));
			break;
		case VK_LEFT:
			MoveCaret(-1);
			break;
		case VK_RIGHT:
			MoveCaret(1);
			break;
		case VK_DELETE:
			EraseCharacter();
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
			HIMC hImc{ ImmGetContext(App::hWnd) };
			int length{ ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0) };

			// 조합중이라면 글자 교체
			if (m_isCompositing && !m_text.empty())
			{
				MoveCaret(-1);
				EraseCharacter();
			}

			// 백스페이스로 조합 중인 글자를 다 지우면 길이가 0이될 수 있음
			m_isCompositing = length ? true : false;
			if (length)
			{
				InsertCharacter(static_cast<wchar_t>(wParam));
				MoveCaret(1);
			}

			ImmReleaseContext(App::hWnd, hImc);
		}
		if (lParam & GCS_RESULTSTR)
		{
			if (m_isCompositing && !m_text.empty())
			{
				MoveCaret(-1);
				EraseCharacter();
			}
			InsertCharacter(static_cast<wchar_t>(wParam));
			MoveCaret(1);
			m_isCompositing = false;
		}
		break;
	}
	}
}

void TextBox::Update(float deltaTime)
{
	if (IsFocus())
	{
		m_caretTimer += deltaTime;
		if (m_caretTimer > 1.0f)
		{
			m_caretTimer -= 1.0f;
			m_drawCaret = !m_drawCaret;
		}
	}
}

void TextBox::Render() const
{
	RenderBackground();
	RenderText();
	RenderCaret();
}

void TextBox::SetFocus(bool focus)
{
	IControl::SetFocus(focus);
	if (focus)
	{
		m_caret = 0;
		MoveCaret(static_cast<int>(m_text.size()));
		m_caretTimer = 0.0f;
		m_drawCaret = true;
	}
}

bool TextBox::IsFocus() const
{
	if (!m_owner)
		return false;
	return m_owner->IsFocus() && m_isFocus;
}

void TextBox::SetMultiLine(bool isMultiLine)
{
	m_isMultiLine = isMultiLine;
}

void TextBox::SetVerticalScroll(bool hasVerticalScroll)
{
	m_hasVerticalScroll = hasVerticalScroll;
}

void TextBox::SetHorizontalScroll(bool hasHorizontalScroll)
{
	m_hasHorizontalScroll = hasHorizontalScroll;
}

void TextBox::UpdateOffset()
{
	auto metrics{ GetTextMetrics(m_caret) };
	int textWidth{ static_cast<int>(metrics.left) };
	int textBoxWidth{ m_size.x - (MARGIN_LEFT + MARGIN_RIGHT) };
	int textBoxHeight{ m_size.y - (MARGIN_TOP + MARGIN_BOTTOM) };

	// 우측 스크롤
	if (textWidth + m_offset.x >= textBoxWidth)
		m_offset.x = textBoxWidth - textWidth;

	// 좌측 스크롤
	else if (textWidth + m_offset.x < 0)
		m_offset.x = -textWidth;

	// 하단 스크롤
	if (metrics.top + metrics.height + m_offset.y > textBoxHeight)
		m_offset.y = textBoxHeight - static_cast<int>(metrics.top + metrics.height);

	// 상단 스크롤
	else if (metrics.top + m_offset.y < 0)
		m_offset.y = static_cast<int>(-metrics.top) + MARGIN_TOP;
}

void TextBox::RenderBackground() const
{
	RECTI rect{ 0, 0, m_size.x, m_size.y };
	rect.Offset(m_position);

	RECTI outline{ rect };
	outline.Offset({ 0, 3 });

	D2D1::ColorF outlineColor{ IsFocus() ? D2D1::ColorF::DeepSkyBlue : D2D1::ColorF::WhiteSmoke };
	//Renderer2D::DrawRoundRect(outline, { 3.0f, 3.0f }, outlineColor);

	D2D1::ColorF color{ IsFocus() ? D2D1::ColorF::White : D2D1::ColorF::Gray };
	//Renderer2D::DrawRoundRect(rect, { 2.0f, 2.0f }, color);
}

void TextBox::RenderText() const
{
	if (!m_textLayout)
		return;

	Graphics::D2D::PushClipRect(
		RECTF{
			MARGIN_LEFT,
			MARGIN_TOP,
			static_cast<float>(m_size.x - MARGIN_RIGHT),
			static_cast<float>(m_size.y - MARGIN_BOTTOM)
		}.Offset(m_position)
	);
	//Renderer2D::DrawText(m_position + INT2{ MARGIN_LEFT, MARGIN_TOP } + m_offset, m_textLayout, D2D1::ColorF::Black);
	Graphics::D2D::PopClipRect();
}

void TextBox::RenderCaret() const
{
	if (!IsFocus() || !m_drawCaret)
		return;

	constexpr auto CARET_WIDTH{ 2 };
	constexpr auto CARET_HEIGHT{ 16 };
	RECTI caret{ 0, 0, CARET_WIDTH, CARET_HEIGHT };
	caret.Offset(m_position);
	caret.Offset({ MARGIN_LEFT, 5 });
	if (m_textLayout)
	{
		auto metrics{ GetTextMetrics(m_caret) };
		caret.Offset({ static_cast<int>(metrics.left), static_cast<int>(metrics.top) });
	}
	caret.Offset({ m_offset.x, m_offset.y });
	Graphics::D2D::DrawRect(caret, Graphics::D2D::Color::Black);
}

void TextBox::SetText(const std::wstring& text)
{
	m_text = text;
	//m_textLayout = Renderer2D::CreateTextLayout(m_text, m_textFormat, m_size.x - MARGIN_LEFT, m_size.y);
	if (!m_isMultiLine)
		m_textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	m_caretTimer = 0.0f;
	m_drawCaret = true;
}

void TextBox::MoveCaret(int count)
{
	m_caret = std::clamp(m_caret + count, 0, static_cast<int>(m_text.size()));
	m_caretTimer = 0.0f;
	m_drawCaret = true;

	UpdateOffset();
}

void TextBox::InsertCharacter(wchar_t character)
{
	m_text.insert(m_caret, 1, character);
	SetText(m_text);
}

void TextBox::EraseCharacter()
{
	if (m_text.empty())
		return;
	m_text.erase(m_caret, 1);
	SetText(m_text);
}

DWRITE_HIT_TEST_METRICS TextBox::GetTextMetrics(int position) const
{
	if (!m_textLayout || position < 0)
		return {};

	FLOAT2 pos{};
	DWRITE_HIT_TEST_METRICS metrics{};
	m_textLayout->HitTestTextPosition(position, TRUE, &pos.x, &pos.y, &metrics);
	return metrics;
}
