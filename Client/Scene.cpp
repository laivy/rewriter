#include "Stdafx.h"
#include "Scene.h"

void Scene::OnCreate() { }
void Scene::OnDestory() { }
void Scene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void Scene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void Scene::Update(FLOAT deltaTime) { }
void Scene::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const { }
void Scene::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const { }
