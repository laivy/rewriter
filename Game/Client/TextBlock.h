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
	std::wstring m_text;
};
