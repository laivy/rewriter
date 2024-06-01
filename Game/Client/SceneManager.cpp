#include "Stdafx.h"
#include "SceneManager.h"
#include "WindowManager.h"

void SceneManager::Update(float deltaTime)
{
	if (auto wm{ WindowManager::GetInstance() })
		wm->Update(deltaTime);
}

void SceneManager::Render2D() const
{
	if (auto wm{ WindowManager::GetInstance() })
		wm->Render();
}

void SceneManager::Render3D() const
{
}
