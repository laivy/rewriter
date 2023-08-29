#pragma once
#include "UI.h"

class Wnd;

class Button : public IUserInterface
{
public:
	Button(const INT2& size);
	virtual ~Button() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

	void SetButtonID(ButtonID id);

private:
	bool m_isMouseOver;
	bool m_isMouseDown;
	ButtonID m_id;
	D2D1::ColorF m_color;
};