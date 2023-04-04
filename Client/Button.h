#pragma once
#include "UI.h"

class Wnd;

class Button : public IUserInterface
{
public:
	Button(FLOAT width, FLOAT height);
	virtual ~Button() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

	void SetCallback(const std::function<void()>& callback);

private:
	std::function<void()> m_callback;
	BOOL m_isMouseOver;
	BOOL m_isMouseDown;
	D2D1::ColorF m_color;
};