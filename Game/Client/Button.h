#pragma once
#include "Control.h"

class Button : public IControl
{
public:
	enum class State
	{
		DEFAULT, MOUSEOVER, MOUSEDOWN
	};

public:
	Button(IWindow* owner);
	~Button() = default;

	virtual void OnMouseEnter(int x, int y) override final;
	virtual void OnMouseLeave(int x, int y) override final;
	virtual void OnMouseEvent(UINT message, int x, int y) override final;

	virtual void Render() const override final;

	void SetText(std::wstring_view text);

public:
	Delegate<> OnButtonClick;

public:
	State m_state;
	ComPtr<IDWriteTextFormat> m_textFormat;
	ComPtr<IDWriteTextLayout> m_textLayout;
	std::wstring m_text;	
};