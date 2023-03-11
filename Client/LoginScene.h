#pragma once
#include "Scene.h"

class Camera;
class Player;

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
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

private:
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Player> m_player;
	std::unique_ptr<Player> m_player2;
};