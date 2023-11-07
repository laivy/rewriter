#pragma once
#include "Scene.h"

class LogoScene :
	public IScene,
	public TSingleton<LogoScene>
{
public:
	LogoScene() = default;
	~LogoScene() = default;

	virtual void OnCreate() final;
	virtual void OnDestory() final;
	virtual void OnLButtonDown(int x, int y) final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) final;

	virtual void Update(FLOAT deltaTime) final;
	virtual void Render2D() const final;
};