#include "Stdafx.h"
#include "App.h"
#include "LoginScene.h"
#include "SceneManager.h"
#include "WindowManager.h"

SceneManager::SceneManager() :
	m_scene{ new LoginScene{} }
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

	UpdateTransition(deltaTime);
}

void SceneManager::Render2D() const
{
	if (m_scene)
		m_scene->Render2D();

	RenderTransition();
}

void SceneManager::Render3D() const
{
	if (m_scene)
		m_scene->Render3D();
}

void SceneManager::UpdateTransition(float deltaTime)
{
	if (!m_transitionData)
		return;

	auto& data{ *m_transitionData };
	if (data.fadeOutTimer < data.fadeOutSeconds)
	{
		data.fadeOutTimer = std::min(data.fadeOutTimer + deltaTime, data.fadeOutSeconds);
		if (data.fadeOutTimer == data.fadeOutSeconds)
			m_scene = data.scene();
	}
	else if (data.fadeInTimer < data.fadeInSeconds)
	{
		data.fadeInTimer = std::min(data.fadeInTimer + deltaTime, data.fadeInSeconds);
		if (data.fadeInTimer == data.fadeInSeconds)
			m_transitionData.reset();
	}
}

void SceneManager::RenderTransition() const
{
	if (!m_transitionData)
		return;

	Graphics::D2D::Color color{};

	const auto& data{ *m_transitionData };
	if (data.fadeOutTimer < data.fadeOutSeconds)
		color.a = data.fadeOutTimer / data.fadeOutSeconds;
	else if (data.fadeInTimer < data.fadeInSeconds)
		color.a = 1.0f - data.fadeInTimer / data.fadeInSeconds;

	Graphics::D2D::DrawRect(RectF{ 0.0f, 0.0f, static_cast<float>(App::size.x), static_cast<float>(App::size.y) }, color);
}
