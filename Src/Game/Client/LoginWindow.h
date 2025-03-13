#pragma once
#include "Window.h"

class LoginWindow final :
	public IWindow,
	public IObserver
{
public:
	LoginWindow();
	~LoginWindow() = default;

private:
	void OnPacket(Packet& packet);
	void OnLoginButtonClicked();
	void OnRegisterButtonClicked();
};
