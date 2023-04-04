#include "Stdafx.h"
#include "Scene.h"

void IScene::OnCreate() { }
void IScene::OnDestory() { }
void IScene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void IScene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void IScene::Update(FLOAT deltaTime) { }
void IScene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const { }
void IScene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }
