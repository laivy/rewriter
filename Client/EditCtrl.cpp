#include "Stdafx.h"
#include "BrushPool.h"
#include "EditCtrl.h"
#include "Font.h"
#include "GameApp.h"
#include "ResourceManager.h"
#include "Wnd.h"

EditCtrl::EditCtrl(const INT2& size) : 
	m_isCompositing{ false },
	m_caretPosition{},
	m_caretRect{},
	m_caretTimer{},
	m_xOffset{}
{
	SetSize(size);
	SetPosition({ 0, 0 });

	if (auto rm{ ResourceManager::GetInstance() })
	{
		SetFont(rm->GetFont(Font::Type::MORRIS12));
		SetText("");
		MoveCaret(0);
	}
}

void EditCtrl::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{

}

void EditCtrl::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_isFocus)
		return;

	switch (message)
	{
	case WM_CHAR:
	{
		switch (wParam)
		{
		case VK_BACK:
			if (!m_text.empty())
				EraseText(1);
			break;
		default:
			InsertText(std::wstring{ static_cast<TCHAR>(wParam) });
			break;
		}
		m_isCompositing = false;
		break;
	}
	case WM_KEYDOWN:
		if (wParam == VK_LEFT)
			MoveCaret(-1);
		else if (wParam == VK_RIGHT)
			MoveCaret(1);
		break;
	case WM_IME_COMPOSITION:
	{
		if (lParam & GCS_COMPSTR)
		{
			HIMC hImc{ ImmGetContext(hWnd) };
			int length{ ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0) };

			// 조합중이라면 글자 교체
			if (m_isCompositing && !m_text.empty())
				EraseText(1);

			// 백스페이스로 조합 중인 글자를 다 지우면 길이가 0이될 수 있음
			m_isCompositing = length ? true : false;
			if (length)
				InsertText(std::wstring{ static_cast<WCHAR>(wParam) });

			ImmReleaseContext(hWnd, hImc);
		}
		if (lParam & GCS_RESULTSTR)
		{
			if (m_isCompositing && !m_text.empty())
				EraseText(1);
			InsertText(std::wstring{ static_cast<WCHAR>(wParam) });
			m_isCompositing = false;
		}
		break;
	}
	}
}

void EditCtrl::Update(FLOAT deltaTime)
{
	if (!m_isFocus)
		return;

	// 캐럿 깜빡임
	m_caretTimer += deltaTime;
	if (m_caretTimer >= CARET_BLINK_SECOND * 2.0f)
		m_caretTimer = 0.0f;
}

void EditCtrl::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (!m_parent)
		return;

	INT2 position{ m_parent->GetPosition(Pivot::LEFTTOP) };
	position += m_position;
	d2dContext->SetTransform(MATRIX::Translation(position.x - m_size.x / 2.0f, position.y - m_size.y / 2.0f));

	// 배경
	D2D1_ROUNDED_RECT rect
	{
		RECTF{ -(CARET_THICKNESS + 1.0f), 0.0f, m_size.x + CARET_THICKNESS + 1.0f, static_cast<float>(m_size.y) },
		1.0f,
		1.0f
	};
	d2dContext->FillRoundedRectangle(rect, BrushPool::GetInstance()->GetBrush(BrushPool::WHITE));

	// 텍스트
	d2dContext->PushAxisAlignedClip(RECTF{ 0.0f, 0.0f, static_cast<float>(m_size.x), static_cast<float>(m_size.y) }, D2D1_ANTIALIAS_MODE_ALIASED);
	d2dContext->DrawTextLayout(FLOAT2{ -m_xOffset, 0.0f }, m_textLayout.Get(), BrushPool::GetInstance()->GetBrush(BrushPool::BLACK));
	d2dContext->PopAxisAlignedClip();

	// 캐럿
	if (m_isFocus && m_parent->IsFocus() && 0.0f <= m_caretTimer && m_caretTimer < CARET_BLINK_SECOND)
		d2dContext->FillRectangle(m_caretRect, BrushPool::GetInstance()->GetBrush(BrushPool::BLACK));
}

void EditCtrl::SetFont(const std::shared_ptr<Font>& font)
{
	m_font = font;
}

void EditCtrl::SetText(const std::string& text)
{
	m_text = TextUtil::str2wstr(text);
	CreateTextLayout();
}

void EditCtrl::EraseText(size_t count)
{
	if (m_caretPosition == 0)
	{
		MoveCaret(0);
		return;
	}

	MoveCaret(-1);
	m_text.erase(m_caretPosition, count);
	CreateTextLayout();
}

void EditCtrl::InsertText(const std::wstring& text)
{
	m_text.insert(m_caretPosition, text);
	CreateTextLayout();
	MoveCaret(static_cast<int>(text.size()));
}

void EditCtrl::MoveCaret(int distance)
{
	m_caretTimer = 0.0f;
	m_caretPosition = std::clamp(m_caretPosition + distance, 0, static_cast<int>(m_text.size()));

	// 캐럿 사각형 범위 설정
	DWRITE_HIT_TEST_METRICS metrics{};
	FLOAT2 pos;
	m_textLayout->HitTestTextPosition(
		m_caretPosition,
		1,
		&pos.x,
		&pos.y,
		&metrics
	);

	// 우측 스크롤
	float caretRightPos{ metrics.left + CARET_THICKNESS / 2.0f };
	float overLength{ caretRightPos - m_size.x };
	if (m_xOffset < overLength)
		m_xOffset = overLength;

	// 좌측 스크롤
	float totalLength{ m_size.x + m_xOffset };
	float caretLeftPos{ std::max(0.0f, metrics.left - CARET_THICKNESS / 2.0f) };
	if (totalLength - caretLeftPos > m_size.x)
		m_xOffset -= metrics.width;
	
	m_xOffset = std::clamp(m_xOffset, 0.0f, totalLength);

	m_caretRect.left = metrics.left - CARET_THICKNESS / 2.0f;
	m_caretRect.right = metrics.left + CARET_THICKNESS / 2.0f;
	m_caretRect.top = pos.y;
	m_caretRect.bottom = pos.y + metrics.height;
	m_caretRect.Offset(-m_xOffset, 0.0f);
	m_caretRect.Offset(1.0f, 0.0f); // 보기 좋게 1px 정도 오른쪽으로
}

void EditCtrl::CreateTextLayout()
{
	auto dwriteFactory{ GameApp::GetInstance()->GetDwriteFactory() };
	dwriteFactory->CreateTextLayout(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_font->GetTextFormat().Get(), m_size.x, m_size.y, &m_textLayout);
	m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
}
