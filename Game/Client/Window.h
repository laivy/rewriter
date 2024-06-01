#pragma once
#include "UI.h"

class IControl;

class IWindow abstract : public IUserInterface
{
public:
	IWindow();
	virtual ~IWindow() = default;

	virtual void OnMouseEvent(UINT message, int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(float deltaTime);
	virtual void Render() const;

protected:
	bool m_isPicked;
	INT2 m_pickPos;
	RECTI m_pickRect;
	std::vector<std::unique_ptr<IControl>> m_controls;
	IControl* m_focusControl;
};