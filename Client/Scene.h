#pragma once

class IScene abstract
{
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void OnCreate();
	virtual void OnDestory();
	virtual void OnResize(int width, int height);
	virtual void OnMouseMove(int x, int y);
	virtual void OnLButtonUp(int x, int y);
	virtual void OnLButtonDown(int x, int y);
	virtual void OnRButtonUp(int x, int y);
	virtual void OnRButtonDown(int x, int y);
	virtual void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;
};