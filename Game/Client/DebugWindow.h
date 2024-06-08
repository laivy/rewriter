#pragma once
#include "Window.h"

class DebugWindow : 
	public IWindow,
	public IObserver
{
public:
	DebugWindow();
	~DebugWindow() = default;

	virtual void OnMouseEvent(UINT message, int x, int y) override final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override final;

	virtual void Update(float deltaTime) override final;
	virtual void Render() const override final;
};