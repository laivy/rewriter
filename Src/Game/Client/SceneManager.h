#pragma once

class IScene;

class SceneManager : public TSingleton<SceneManager>
{
private:
	struct TransitionData
	{
		float fadeInSeconds{ 0.0f };
		float fadeOutSeconds{ 0.0f };
		float fadeInTimer{ 0.0f };
		float fadeOutTimer{ 0.0f };
		std::function<std::unique_ptr<IScene>()> scene{};
	};

public:
	SceneManager();
	~SceneManager();

	void Update(float deltaTime);
	void Render2D() const;
	void Render3D() const;

	template<class T>
	requires std::is_base_of_v<IScene, T>
	void Transition(float fadeInSeconds, float fadeOutSeconds)
	{
		if (m_transitionData)
		{
			assert(false && "TRANSITION IS ALREADY RUNNING");
			return;
		}

		TransitionData data{};
		data.fadeInSeconds = fadeInSeconds;
		data.fadeOutSeconds = fadeOutSeconds;
		data.scene = []() { return std::make_unique<T>(); };
		m_transitionData = data;
	}

private:
	void UpdateTransition(float deltaTime);
	void RenderTransition() const;

private:
	std::unique_ptr<IScene> m_scene;
	std::optional<TransitionData> m_transitionData;
};
