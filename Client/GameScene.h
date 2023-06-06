#pragma once
#include "Scene.h"

class Camera;
class Map;
class Player;

class GameScene : public IScene, public TSingleton<GameScene>
{
public:
	GameScene();
	~GameScene();

	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

	Map* GetMap() const;

private:
	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<Player> m_player;
	std::unique_ptr<Map> m_map;
};