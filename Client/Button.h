#pragma once
#include "UI.h"

class Wnd;

class Button : public UI
{
public:
	Button(FLOAT width, FLOAT height, FLOAT x = 0.0f, FLOAT y = 0.0f);
	virtual ~Button() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y);

	virtual void Update(FLOAT deltaTime) { }
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const;

	virtual RECTF GetRect() const;

	void SetPosition(const FLOAT2& position);

	FLOAT2 GetPosition() const;
};