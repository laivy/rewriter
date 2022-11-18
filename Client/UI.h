#pragma once
#include "Stdafx.h"
#include "GameObject.h"

class Wnd;

class UI abstract : public GameObject
{
public:
	UI();
	virtual ~UI() = default;

	virtual void OnMouseEvent(HWND hWnd, UINT message, INT x, INT y) { }

	void SetSize(const FLOAT2& size);
	void SetParent(Wnd* wnd);

	RECTF GetRect() const;
	FLOAT2 GetSize() const;
	Wnd* const GetParent() const;

protected:
	FLOAT2 m_size; // 가로, 세로

private:
	Wnd* m_parent; // 부모 윈도우
};