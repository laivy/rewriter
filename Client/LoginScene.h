#pragma once
#include "Scene.h"

class NytProperty;

class LoginScene : public Scene, public TSingleton<LoginScene>
{
public:
	LoginScene() = default;
	~LoginScene() = default;

	virtual void OnCreate();
	virtual void OnDestory();

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const;

private:
	NytProperty* m_prop;
};