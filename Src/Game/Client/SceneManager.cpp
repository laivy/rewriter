#include "Stdafx.h"
#include "LoginScene.h"
#include "SceneManager.h"
#include "WindowManager.h"

SceneManager::SceneManager() :
	m_scene{ new LoginScene }
{
}

SceneManager::~SceneManager()
{
	WindowManager::Destroy();
}

void SceneManager::Update(float deltaTime)
{
	if (m_scene)
		m_scene->Update(deltaTime);
}

void SceneManager::Render2D() const
{
	if (m_scene)
		m_scene->Render2D();
}

void SceneManager::Render3D() const
{
	if (m_scene)
		m_scene->Render3D();
}
