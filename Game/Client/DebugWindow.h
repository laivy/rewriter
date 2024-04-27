#pragma once
#include "Window.h"

class DebugWindow : public IWindow
{
public:
	DebugWindow() = default;
	~DebugWindow() = default;

	virtual void Update(float deltaTime) override final;
	virtual void Render() const override final;
};