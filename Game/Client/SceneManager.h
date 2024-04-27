#pragma once
#include "Scene.h"

class SceneManager : public TSingleton<SceneManager>
{
public:
	SceneManager() = default;
	~SceneManager() = default;

	void Update(float deltaTime);
	void Render2D() const;
	void Render3D() const;

private:
	std::unique_ptr<IScene> m_scene;
};