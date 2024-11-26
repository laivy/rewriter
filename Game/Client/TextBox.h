#pragma once
#include "Control.h"

class TextBox final : public IControl
{
private:
	using Visual = std::variant<std::shared_ptr<Resource::Sprite>, std::tuple<INT2, int32_t, int32_t>>;

public:
	TextBox(IWindow* owner, const std::shared_ptr<Resource::Property>& prop = nullptr);
	~TextBox() = default;

	void Update(float deltaTime) override final;
	void Render() const override final;

	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override final;

	void SetFocus(bool focus) override final;

	bool IsFocus() const override final;

	std::wstring GetText() const;

private:
	void Build(const std::shared_ptr<Resource::Property>& prop);

	void RenderBackground() const;
	void RenderText() const;
	void RenderCaret() const;

	void SetText(std::wstring_view text);
	void MoveCaret(int distance);
	void UpdateScrollOffset();
	void InsertCharacter(wchar_t character);
	void EraseCharacter();

private:
	static constexpr auto PADDING_LEFT = 5;
	static constexpr auto PADDING_TOP = 2;
	static constexpr auto PADDING_RIGHT = 5;
	static constexpr auto PADDING_BOTTOM = 2;

	std::array<Visual, 2> m_visuals;

	std::wstring m_text;
	Graphics::D2D::Font m_font;
	Graphics::D2D::TextMetrics m_metrics;

	bool m_isMultiLine;
	bool m_isPassword;

	bool m_onCompositing;
	bool m_isCaretVisible;
	float m_caretTimer;
	int m_caretPosition;
	INT2 m_scrollOffset;
};
