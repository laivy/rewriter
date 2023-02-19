#pragma once
#include "Scene.h"

class Camera;
class NytProperty;
class NytImage;

class LoginScene : public Scene, public TSingleton<LoginScene>
{
public:
	LoginScene();
	~LoginScene() = default;

	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

private:
	NytProperty* m_prop;
	std::unique_ptr<NytImage> m_paimon;
	std::unique_ptr<Camera> m_camera;
};