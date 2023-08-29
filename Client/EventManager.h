#pragma once

class IScene;

class EventManager : public TSingleton<EventManager>
{
private:
	template <class... Args>
	class Event
	{
	public:
		Event() = default;
		~Event() = default;

		void operator()(const Args&... args)
		{
			std::erase_if(m_callbacks, [&](const auto& f) { return f(args...); });
		}

		friend void operator+=(Event& event, const std::function<bool(Args...)>& callback)
		{
			event.m_callbacks.push_back(callback);
		}

		void Clear()
		{
			m_callbacks.clear();
		}

	private:
		std::vector<std::function<bool(Args...)>> m_callbacks;
	};

public:
	EventManager() = default;
	~EventManager() = default;

	void Clear();

public:
	Event<IScene*> OnSceneChange;
};