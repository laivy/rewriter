#pragma once
#include "UI.h"
#include "FontPool.h"

class EditCtrl : public UI
{
public:
	EditCtrl(FLOAT width, FLOAT height, FLOAT x = 0.0f, FLOAT y = 0.0f);
	~EditCtrl() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);

	virtual void Update(FLOAT deltaTime) { }
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;
	virtual RECTF GetRect() const;

	void SetText(const std::wstring& text);
	void SetFont(FontPool::Type fontType);

private:
	mutable std::mutex m_mutex;
	ComPtr<IDWriteTextLayout> m_textLayout;
	ComPtr<IDWriteTextFormat> m_textFormat;
	std::wstring m_text;
};