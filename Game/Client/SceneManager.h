#pragma once
#include "EventManager.h"
#include "Common/Time.h"

class Scene;

class SceneManager : public TSingleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

	void Update(float deltaTime);
	void Render2D() const;
	void Render3D(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetScene(IScene* scene);
	void SetFadeIn(FLOAT second, const std::function<void()>& callback = []() {});
	void SetFadeOut(FLOAT second, const std::function<void()>& callback = []() {});

private:
	void OnResize(int width, int height);
	void OnKeyboardEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void OnMouseEvent(UINT message, int x, int y);

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