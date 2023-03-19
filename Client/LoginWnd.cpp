#include "Stdafx.h"
#include "LoginWnd.h"
#include "Button.h"

LoginWnd::LoginWnd(FLOAT width, FLOAT height) : Wnd{ width, height }
{
	Button* loginBtn{ new Button{ 200.0f, 20.0f } };
	loginBtn->SetPosition(FLOAT2{ width / 2.0f, height * 0.9f }, Pivot::CENTER);
	AddUI(loginBtn);
}