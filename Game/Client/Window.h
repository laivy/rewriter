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
	RECTI m_titleBarRect;
	INT2 m_pickPos;
	bool m_isPicked;

	std::vector<std::unique_ptr<IControl>> m_controls;
	IControl* m_focusControl;
};