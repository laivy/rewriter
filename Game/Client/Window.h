#pragma once
#include "UI.h"

class IControl;

class IWindow abstract : public IUserInterface
{
public:
	IWindow();
	virtual ~IWindow() = default;

	virtual void OnMouseLeave(int x, int y) override;
	virtual void OnMouseEvent(UINT message, int x, int y) override;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void Update(float deltaTime);
	virtual void Render() const;

	void Register(const std::shared_ptr<IControl>& control);

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