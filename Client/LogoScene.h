#pragma once
#include "Scene.h"

class NytProperty;

class LogoScene : public IScene, public TSingleton<LogoScene>
{
public:
	LogoScene();
	~LogoScene();

	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

private:
	NytProperty* m_prop;
	BOOL m_isFirstUpdate;
};