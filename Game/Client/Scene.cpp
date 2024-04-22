#include "Stdafx.h"
#include "Scene.h"

void IScene::OnCreate() { }
void IScene::OnDestroy() { }
void IScene::OnResize(int width, int height) { }
void IScene::OnMouseMove(int x, int y) { }
void IScene::OnLButtonUp(int x, int y) { }
void IScene::OnLButtonDown(int x, int y) { }
void IScene::OnRButtonUp(int x, int y) { }
void IScene::OnRButtonDown(int x, int y) { }
void IScene::OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam) { }
void IScene::Update(float deltaTime) { }
void IScene::Render3D() const { }
void IScene::Render2D() const { }
