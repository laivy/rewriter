#include "Stdafx.h"
#include "Scene.h"
#include "WindowManager.h"

IScene::IScene()
{
	WindowManager::Destroy();
	WindowManager::Instantiate();
}

void IScene::Update(float deltaTime)
{
	if (auto wm{ WindowManager::GetInstance() })
		wm->Update(deltaTime);
}

void IScene::Render2D() const
{
	if (auto wm{ WindowManager::GetInstance() })
		wm->Render();
}

void IScene::Render3D() const
{
}