#pragma once

class EventManager : public TSingleton<EventManager>
{
private:
	class Event
	{
	public:
		Event() = default;
		~Event() = default;

		void Clear();

		void operator()();

		friend void operator+=(Event& event, const std::function<void()>& callback)
		{
			event.m_callbacks.push_back(callback);
		}

	private:
		std::vector<std::function<void()>> m_callbacks;
	};

public:
	EventManager() = default;
	~EventManager() = default;

	void Clear();

public:
	Event OnSceneChange;
};