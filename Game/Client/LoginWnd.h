#pragma once
#include "Wnd.h"

class LoginWnd : public Wnd
{
public:
	LoginWnd(const INT2& size);
	~LoginWnd() = default;

	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) final;
};