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

private:
	void UpdateMouseOverControl(int x, int y);
	void UpdateFocusControl(int x, int y);

protected:
	RECTI m_titleBarRect;
	INT2 m_pickPos;
	bool m_isPicked;

	std::vector<std::shared_ptr<IControl>> m_controls;
	std::weak_ptr<IControl> m_mouseOverControl;
	std::weak_ptr<IControl> m_focusControl;
};