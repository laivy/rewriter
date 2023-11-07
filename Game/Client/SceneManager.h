#pragma once
#include "EventManager.h"
#include "Game/Common/Time.h"

class Scene;

class SceneManager : public TSingleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

	void OnResize(int width, int height);
	void OnMouseMove(int x, int y);
	void OnLButtonUp(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnRButtonUp(int x, int y);
	void OnRButtonDown(int x, int y);
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);

	void Update(float deltaTime);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Render2D() const;

	void SetFadeIn(FLOAT second, const std::function<void()>& callback = []() {});
	void SetFadeOut(FLOAT second, const std::function<void()>& callback = []() {});

	template<class T>
	requires std::is_base_of_v<IScene, T>
	void SetScene(T* scene)
	{
		if (auto em{ EventManager::GetInstance() })
			em->OnSceneChange(scene);
		if (m_scene)
			m_scene->OnDestroy();
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
		Time start{};
		Time end{};
		FLOAT alpha{ 1.0f };
	};

private:
	IScene* m_scene;
	FadeInfo m_fadeInfo;
};