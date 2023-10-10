#pragma once
#include "UI.h"
#include "Font.h"

class Label : public IUserInterface
{
public:
	Label(const INT2& size);
	~Label() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	void SetFont(const std::shared_ptr<Font>& font);
	void SetText(const std::string& text);

private:
	std::wstring m_text;
	std::shared_ptr<Font> m_font;
	ComPtr<IDWriteTextLayout> m_textLayout;
};