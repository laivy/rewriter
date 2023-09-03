#include "Stdafx.h"
#include "Scene.h"

void IScene::OnCreate() { }
void IScene::OnDestory() { }
void IScene::OnResize(int width, int height) { }
void IScene::OnMouseMove(int x, int y) { }
void IScene::OnLButtonUp(int x, int y) { }
void IScene::OnLButtonDown(int x, int y) { }
void IScene::OnRButtonUp(int x, int y) { }
void IScene::OnRButtonDown(int x, int y) { }
void IScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) { }
void IScene::Update(FLOAT deltaTime) { }
void IScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const { }
void IScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }
