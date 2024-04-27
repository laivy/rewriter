#pragma once
#include "UI.h"

class IWindow abstract : public IUserInterface
{
public:
	IWindow() = default;
	virtual ~IWindow() = default;

	virtual void OnMouseEvent(UINT message, int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(float deltaTime);
	virtual void Render() const;

private:
	std::vector<std::unique_ptr<IUserInterface>> m_userInterfaces;
};