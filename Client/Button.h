#pragma once
#include "UI.h"

class Wnd;

class Button : public IUserInterface
{
public:
	Button(const INT2& size);
	virtual ~Button() = default;

	virtual void OnMouseMove(int x, int y) final;
	virtual void OnLButtonUp(int x, int y) final;
	virtual void OnLButtonDown(int x, int y) final;

	virtual void Update(FLOAT deltaTime) final;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const final;

	void SetButtonID(ButtonID id);

private:
	ButtonID m_id;
	bool m_isMouseOver;
	bool m_isMouseDown;
	D2D1::ColorF m_color;
};