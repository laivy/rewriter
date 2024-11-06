#pragma once
#include "Window.h"

class LoginWindow : 
	public IWindow,
	public IObserver
{
public:
	LoginWindow();
	~LoginWindow() = default;

	virtual void OnMouseEvent(UINT message, int x, int y) override final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override final;

	virtual void Update(float deltaTime) override final;
	virtual void Render() const override final;

private:
	void OnPacket(Packet& packet);
	void OnLoginButtonClicked();
	void OnRegisterButtonClicked();
};
