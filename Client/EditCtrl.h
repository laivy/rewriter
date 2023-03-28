#pragma once
#include "UI.h"
#include "FontPool.h"

class EditCtrl : public UI
{
public:
	EditCtrl(FLOAT width, FLOAT height, FontPool::Type fontType = FontPool::Type::MORRIS);
	~EditCtrl() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void SetText(const std::wstring& text);

private:
	void EraseText(size_t count);
	void InsertText(const std::wstring& text);
	void MoveCaret(int distance);

private:
	ComPtr<IDWriteTextLayout> m_textLayout;
	ComPtr<IDWriteTextFormat> m_textFormat;

	std::wstring m_text;
	BOOL m_isCompositing;

	constexpr static INT CARET_THICKNESS = 2;
	constexpr static FLOAT CARET_BLINK_SECOND = 0.5f;
	INT m_caretPosition;
	RECTF m_caretRect;
	FLOAT m_caretTimer;
	FLOAT m_xOffset;
};