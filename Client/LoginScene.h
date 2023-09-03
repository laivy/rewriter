#pragma once
#include "Scene.h"

class Camera;
class Player;

class LoginScene : 
	public IScene, 
	public TSingleton<LoginScene>
{
public:
	LoginScene() = default;
	~LoginScene() = default;

	virtual void OnCreate() final;
	virtual void OnDestory() final;
	virtual void OnMouseMove(int x, int y) final;
	virtual void OnLButtonDown(int x, int y) final;
	virtual void OnLButtonUp(int x, int y) final;
	virtual void OnRButtonDown(int x, int y) final;
	virtual void OnRButtonUp(int x, int y) final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) final;

	virtual void Update(FLOAT deltaTime) final;
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const final;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const final;
};