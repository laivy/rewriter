#pragma once
#include "Control.h"

class TextBlock : public IControl
{
public:
	TextBlock(IWindow* owner);
	~TextBlock() = default;

	virtual void Render() const override final;

	void SetText(std::wstring_view text);

private:
	ComPtr<IDWriteTextFormat> m_textFormat;
	ComPtr<IDWriteTextLayout> m_textLayout;
	std::wstring m_text;
};