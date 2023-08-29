#pragma once
#include "Wnd.h"

class LoginWnd : public Wnd
{
public:
	LoginWnd(const INT2& size);
	~LoginWnd() = default;

	virtual void OnButtonClick(ButtonID id);
};