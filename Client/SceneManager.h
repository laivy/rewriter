#pragma once
#include "Scene.h"

class SceneManager : public TSingleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager() = default;

	void OnDestroy();
	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

	template<class T>
	requires std::is_base_of_v<Scene, T>
	void SetScene(T* scene)
	{
		if (m_scene)
			m_scene->OnDestory();
		m_scene = scene;
	}

	template<class T>
	requires std::is_base_of_v<Scene, T>
	void ChangeScene(T* scene)
	{
		m_nextScene = scene;
	}

private:
	Scene* m_scene;
	Scene* m_nextScene;
};