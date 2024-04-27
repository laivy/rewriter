#pragma once
#include <any>

class IObserver;

class IEvent abstract
{
public:
	virtual void Unregister(IObserver* observer) = 0;
};

class IObserver
{
protected:
	IObserver() : m_event{ nullptr }
	{
	}

public:
	~IObserver()
	{
		if (m_event)
			m_event->Unregister(this);
	}

public:
	IEvent* m_event;
};

template <class... Params>
class Event : public IEvent
{
private:
	using Callback = std::function<void(Params...)>;

public:
	Event() = default;
	~Event()
	{
		for (const auto& [o, _] : m_observers)
		{
			if (o)
				o->m_event = nullptr;
		}
	}

	void Register(const Callback& callback)
	{
		m_observers[nullptr].push_back(callback);
	}

	void Register(IObserver* observer, const Callback& callback)
	{
		observer->m_event = this;
		m_observers[observer].push_back(callback);
	}

	virtual void Unregister(IObserver* observer) override final
	{
		m_observers.erase(observer);
	}

	void Notify(const Params&... params)
	{
		for (const auto& [_, callbacks] : m_observers)
		{
			for (const auto& callback : callbacks)
				callback(params...);
		}
	}

private:
	std::map<IObserver*, std::vector<Callback>> m_observers;
};