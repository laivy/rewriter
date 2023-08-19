#include "Stdafx.h"
#include "BrushPool.h"
#include "Camera.h"
#include "GameScene.h"
#include "LoginScene.h"
#include "Player.h"
#include "Scene.h"
#include "SceneManager.h"

SceneManager::SceneManager() : m_scene{ nullptr }
{

}

void SceneManager::OnCreate()
{
	LoginScene::Instantiate();
	SetScene(LoginScene::GetInstance());
}

void SceneManager::OnDestroy()
{
	if (m_scene)
		m_scene->OnDestory();
}

void SceneManager::OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_scene)
		m_scene->OnMouseEvent(hWnd, message, wParam, lParam);
}

void SceneManager::OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_scene)
		m_scene->OnKeyboardEvent(hWnd, message, wParam, lParam);
}

void SceneManager::Update(FLOAT deltaTime)
{
	if (m_scene)
		m_scene->Update(deltaTime);

	UpdateFadeEffect();
}

void SceneManager::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	if (m_scene)
		m_scene->Render(commandList);
}

void SceneManager::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (m_scene)
		m_scene->Render(d2dContext);

	RenderFadeEffect(d2dContext);
}

void SceneManager::SetFadeIn(FLOAT second, const std::function<void()>& callback)
{
	m_fadeInfo.type = FadeType::FADEIN;
	m_fadeInfo.startTime = std::chrono::high_resolution_clock::now();
	m_fadeInfo.endTime = m_fadeInfo.startTime + std::chrono::milliseconds{ static_cast<UINT>(second * 1000) };
	m_fadeInfo.callback = callback;
	m_fadeInfo.alpha = 1.0f;
}

void SceneManager::SetFadeOut(FLOAT second, const std::function<void()>& callback)
{
	m_fadeInfo.type = FadeType::FADEOUT;
	m_fadeInfo.startTime = std::chrono::high_resolution_clock::now();
	m_fadeInfo.endTime = m_fadeInfo.startTime + std::chrono::milliseconds{ static_cast<UINT>(second * 1000) };
	m_fadeInfo.callback = callback;
	m_fadeInfo.alpha = 1.0f;
}

void SceneManager::UpdateFadeEffect()
{
	using namespace std::chrono;
	using clock = high_resolution_clock;

	if (m_fadeInfo.type == FadeType::NONE)
		return;

	auto curr{ clock::now() };
	milliseconds fadeDuration{ duration_cast<milliseconds>(m_fadeInfo.endTime - m_fadeInfo.startTime) }; // 페이드가 진행되는 총 시간
	milliseconds timePassed{ duration_cast<milliseconds>(curr - m_fadeInfo.startTime) }; // 페이드 시작 후 지금까지 흐른 시간

	if (m_fadeInfo.type == FadeType::FADEIN)
		m_fadeInfo.alpha = 1.0f - static_cast<float>(timePassed.count()) / static_cast<float>(fadeDuration.count());
	else if (m_fadeInfo.type == FadeType::FADEOUT)
		m_fadeInfo.alpha = static_cast<float>(timePassed.count()) / static_cast<float>(fadeDuration.count());

	if (curr > m_fadeInfo.endTime)
	{
		// 콜백에서 m_fadeInfo를 변경할 수 있기 때문에 초기화 후 콜백 함수를 호출한다.
		auto callback{ m_fadeInfo.callback };
		m_fadeInfo = {};
		callback();
	}
}

void SceneManager::RenderFadeEffect(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	if (m_fadeInfo.type == FadeType::NONE)
		return;

	auto brush{ BrushPool::GetInstance()->GetBrush(BrushPool::BLACK) };
	brush->SetOpacity(m_fadeInfo.alpha);
	d2dContext->SetTransform(MATRIX::Identity());
	d2dContext->FillRectangle(RECTF{ 0.0f, 0.0f, 1920.0f, 1080.0f }, brush);
	brush->SetOpacity(1.0f);
}
