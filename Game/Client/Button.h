#pragma once
#include "Control.h"

class Button : public IControl
{
public:
	enum class State
	{
		Enable,
		MouseOver,
		MouseDown,
		Disable
	};

public:
	Button(IWindow* owner);
	~Button() = default;

	virtual void OnMouseEnter(int x, int y) override final;
	virtual void OnMouseLeave(int x, int y) override final;
	virtual void OnMouseEvent(UINT message, int x, int y) override final;

	virtual void Render() const override final;

public:
	Delegate<> OnButtonClick;

public:
	State m_state;
};
