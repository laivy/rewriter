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

	void SetOnButtonClick(const std::function<void()>& callback);

private:
	std::function<void()> m_onButtonClick;
	BOOL m_isMouseOver;
	BOOL m_isMouseDown;
	D2D1::ColorF m_color;
};