#pragma once
#include "Scene.h"

class Property;

class LogoScene :
	public IScene,
	public TSingleton<LogoScene>
{
public:
	LogoScene();
	~LogoScene() = default;

	virtual void OnCreate() final;
	virtual void OnDestory() final;
	virtual void OnLButtonDown(int x, int y) final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) final;

	virtual void Update(FLOAT deltaTime) final;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const final;

private:
	Property* m_prop;
};