#pragma once
#include "Control.h"

class TextBox : public IControl
{
public:
	TextBox(IWindow* owner);
	~TextBox() = default;

	virtual void OnMouseEvent(UINT message, int x, int y) override final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override final;

	virtual void Update(float deltaTime) override final;
	virtual void Render() const override final;

	virtual void SetFocus(bool focus) override final;
	virtual bool IsFocus() const override final;

	void SetMultiLine(bool isMultiLine);
	void SetVerticalScroll(bool hasVerticalScroll);
	void SetHorizontalScroll(bool hasHorizontalScroll);

private:
	void UpdateOffset();

	void RenderBackground() const;
	void RenderText() const;
	void RenderCaret() const;

	void SetText(const std::wstring& text);
	void MoveCaret(int count);
	void InsertCharacter(wchar_t character);
	void EraseCharacter();

	DWRITE_HIT_TEST_METRICS GetTextMetrics(int position) const;

private:
	static constexpr auto MARGIN_LEFT = 5;
	static constexpr auto MARGIN_TOP = 2;
	static constexpr auto MARGIN_RIGHT = 5;
	static constexpr auto MARGIN_BOTTOM = 2;

	ComPtr<IDWriteTextFormat> m_textFormat;
	ComPtr<IDWriteTextLayout> m_textLayout;
	std::wstring m_text;
	bool m_isCompositing;

	int m_caret;
	float m_caretTimer;
	bool m_drawCaret;

	INT2 m_offset;
	bool m_isMultiLine;
	bool m_hasVerticalScroll;
	bool m_hasHorizontalScroll;
};