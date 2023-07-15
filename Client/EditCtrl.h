#pragma once
#include "UI.h"
#include "Font.h"

class EditCtrl : public IUserInterface
{
private:
	constexpr static int CARET_THICKNESS = 2;
	constexpr static float CARET_BLINK_SECOND = 0.5f;

public:
	EditCtrl(FLOAT width, FLOAT height, Font::Type fontType = Font::Type::MORRIS12);
	~EditCtrl() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void SetFont(const std::shared_ptr<Font>& font);
	void SetText(const std::string& text);

private:
	void EraseText(size_t count);
	void InsertText(const std::wstring& text);
	void MoveCaret(int distance);
	void CreateTextLayout();

private:
	std::shared_ptr<Font> m_font;
	ComPtr<IDWriteTextLayout> m_textLayout;

	std::wstring m_text;
	bool m_isCompositing;

	// 캐럿
	int m_caretPosition;
	RECTF m_caretRect;
	float m_caretTimer;
	float m_xOffset;
};