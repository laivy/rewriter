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

	void SetParent(Wnd* wnd);

	RECTF GetRect() const;
	Wnd* const GetParent() const;

protected:
	Wnd* m_parent; // 부모 윈도우
};