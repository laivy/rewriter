#pragma once
#include "Scene.h"

class GameScene : 
	public IScene,
	public TSingleton<GameScene>
{
public:
	GameScene() = default;
	~GameScene() = default;

	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;
};