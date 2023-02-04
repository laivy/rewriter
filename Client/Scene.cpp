#include "Stdafx.h"
#include "Scene.h"

void Scene::OnCreate() { }
void Scene::OnDestory() { }
void Scene::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void Scene::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) { }
void Scene::Update(FLOAT deltaTime) { }
void Scene::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const { }
