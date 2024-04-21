#pragma once

template <class... Params>
class Event;

template <class... Params>
class Observer
{
public:
	friend class Event<Params...>;

	Observer(Event<Params...>* event, const std::function<void(Params...)>& callback) :
		m_event{ event },
		m_callback{ callback }
	{
	}

	~Observer()
	{
		if (m_event)
		{
			m_event->Remove(this);
			m_event = nullptr;
		}
	}

private:
	void OnNotify(const Params&... param)
	{
		if (m_callback)
			m_callback(param...);
	}

private:
	Event<Params...>* m_event;
	std::function<void(Params...)> m_callback;
};

template <class... Params>
class Event
{
public:
	Event() = default;
	~Event() = default;

	void Notify(const Params&... params)
	{
		for (const auto& o : m_observers)
			o->OnNotify(params...);
	}

	std::unique_ptr<Observer<Params...>> Add(const std::function<void(Params...)>& callback)
	{
		auto observer{ std::make_unique<Observer<Params...>>(this, callback) };
		m_observers.push_back(observer.get());
		return observer;
	}

	void Remove(Observer<Params...>* observer)
	{
		std::erase(m_observers, observer);
	}

private:
	std::vector<Observer<Params...>*> m_observers;
};