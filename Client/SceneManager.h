#pragma once
#include "Scene.h"
#include "EventManager.h"

class SceneManager : public TSingleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager() = default;

	void OnCreate();
	void OnDestroy();
	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

	void SetFadeIn(FLOAT second, const std::function<void()>& callback = []() {});
	void SetFadeOut(FLOAT second, const std::function<void()>& callback = []() {});

	template<class T>
	requires std::is_base_of_v<IScene, T>
	void SetScene(T* scene)
	{
		if (m_scene)
			m_scene->OnDestory();
		if (auto em{ EventManager::GetInstance() })
			em->OnSceneChange();
		if (scene)
			scene->OnCreate();
		m_scene = scene;
	}

private:
	void UpdateFadeEffect();
	void RenderFadeEffect(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

private:
	enum class FadeType { NONE, FADEIN, FADEOUT };
	struct FadeInfo
	{
		FadeType type{ FadeType::NONE };
		std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
		std::function<void()> callback{};
		FLOAT alpha{ 1.0f };
	};

private:
	IScene* m_scene;
	FadeInfo m_fadeInfo;
};