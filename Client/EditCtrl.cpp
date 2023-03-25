#include "Stdafx.h"
#include "BrushPool.h"
#include "EditCtrl.h"
#include "FontPool.h"
#include "NytApp.h"
#include "TextUtil.h"
#include "Wnd.h"

EditCtrl::EditCtrl(FLOAT width, FLOAT height, FontPool::Type fontType) : 
	m_caretPosition{},
	m_caretRect{},
	m_caretTimer{}
{
	SetSize(FLOAT2{ width, height });
	SetPosition(FLOAT2{ 0.0f, 0.0f });

	if (auto fp{ FontPool::GetInstance() })
	{
		m_textFormat = fp->GetFont(fontType);
		SetText(L"");
	}
}

void EditCtrl::OnMouseEvent(HWND hWnd, UINT message, INT x, INT y)
{

}

void EditCtrl::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 한글을 조합중인지
	static BOOL isCompositing{ FALSE };
	
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
		isCompositing = FALSE;
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
			int len{ ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0) };

			// 조합중이라면 글자 교체
			if (isCompositing && !m_text.empty())
				EraseText(1);

			// 백스페이스로 조합 중인 글자를 다 지우면 길이가 0이될 수 있음
			isCompositing = len ? TRUE : FALSE;
			if (len)
				InsertText(std::wstring{ static_cast<WCHAR>(wParam) });

			ImmReleaseContext(hWnd, hImc);
		}
		if (lParam & GCS_RESULTSTR)
		{
			if (isCompositing && !m_text.empty())
				EraseText(1);
			InsertText(std::wstring{ static_cast<WCHAR>(wParam) });
			isCompositing = FALSE;
		}
		break;
	}
	}
}

void EditCtrl::Update(FLOAT deltaTime)
{
	m_caretTimer += deltaTime;

	// 캐럿 깜빡임
	if (m_caretTimer >= CARET_BLINK_SECOND * 2.0f)
	{
		m_caretTimer = 0.0f;
	}
	if (m_caretTimer > CARET_BLINK_SECOND)
	{
		m_caretRect = RECTF{};
		return;
	}

	// 캐럿 사각형 범위 설정
	DWRITE_HIT_TEST_METRICS metrics{};
	FLOAT2 pos;
	m_textLayout->HitTestTextPosition(
		m_caretPosition - 1,
		1,
		&pos.x,
		&pos.y,
		&metrics
	);

	m_caretRect.left = pos.x - CARET_THICKNESS / 2.0f;
	m_caretRect.right = pos.x + CARET_THICKNESS / 2.0f;
	m_caretRect.top = pos.y;
	m_caretRect.bottom = pos.y + metrics.height;
	m_caretRect.Offset(1.0f, 0.0f);
}

void EditCtrl::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (!m_parent || !m_textFormat) return;

	FLOAT2 position{ m_position };
	position += m_parent->GetPosition();
	d2dContext->SetTransform(MATRIX::Translation(position.x, position.y));

	// 배경
	d2dContext->FillRectangle(RECTF{ 0.0f, 0.0f, m_size.x, m_size.y }, BrushPool::GetInstance()->GetBrush(BrushPool::WHITE));

	// 텍스트
	std::unique_lock lock{ m_mutex };
	d2dContext->DrawTextLayout(FLOAT2{ 0.0f, 0.0f }, m_textLayout.Get(), BrushPool::GetInstance()->GetBrush(BrushPool::BLACK), D2D1_DRAW_TEXT_OPTIONS_CLIP);

	// 캐럿
	d2dContext->FillRectangle(m_caretRect, BrushPool::GetInstance()->GetBrush(BrushPool::GREEN));
}

void EditCtrl::SetText(const std::wstring& text)
{
	m_text = text;

	auto dwriteFactory{ NytApp::GetInstance()->GetDwriteFactory() };
	dwriteFactory->CreateTextLayout(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_textFormat.Get(), m_size.x, m_size.y, &m_textLayout);
	m_textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void EditCtrl::EraseText(size_t count)
{
	MoveCaret(-1);
	m_text.erase(m_caretPosition, count);
	SetText(m_text);
}

void EditCtrl::InsertText(const std::wstring& text)
{
	m_text.insert(m_caretPosition, text);
	SetText(m_text);
	MoveCaret(text.size());
}

void EditCtrl::MoveCaret(int distance)
{
	m_caretTimer = 0.0f;
	m_caretPosition = std::clamp(m_caretPosition + distance, static_cast<size_t>(0), m_text.size());
}
