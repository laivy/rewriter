#pragma once
#include "UI.h"

class Label : public IUserInterface
{
public:
	Label(FLOAT width, FLOAT height);
	~Label() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void SetText(const std::wstring& text);

private:
	std::wstring m_text;
	ComPtr<IDWriteTextLayout> m_textLayout;
	ComPtr<IDWriteTextFormat> m_textFormat;
};