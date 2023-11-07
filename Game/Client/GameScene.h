#pragma once
#include "Scene.h"

class GameScene : 
	public IScene,
	public TSingleton<GameScene>
{
public:
	GameScene() = default;
	~GameScene() = default;

	virtual void OnCreate() final;
	virtual void OnDestroy() final;
	virtual void OnResize(int width, int height) final;
	virtual void OnMouseMove(int x, int y) final;
	virtual void OnLButtonDown(int x, int y) final;
	virtual void OnLButtonUp(int x, int y) final;
	virtual void OnRButtonDown(int x, int y) final;
	virtual void OnRButtonUp(int x, int y) final;
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) final;

	virtual void Update(FLOAT deltaTime) final;
	virtual void Render3D(const ComPtr<ID3D12GraphicsCommandList>& commandList) const final;
	virtual void Render2D() const final;
};