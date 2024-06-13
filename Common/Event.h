#pragma once

class IObserver;

class IEvent abstract : public std::enable_shared_from_this<IEvent>
{
public:
	virtual void Unregister(IObserver* observer) = 0;
};

class IObserver
{
protected:
	IObserver() = default;

public:
	~IObserver()
	{
		for (const auto& event : m_events)
		{
			if (event.expired())
				continue;
			if (auto e{ event.lock() })
				e->Unregister(this);
		}
	}

	void Register(std::weak_ptr<IEvent> event)
	{
		if (std::ranges::find_if(m_events, [&event](const auto& e) { return e.lock() == event.lock(); }) == m_events.end())
			m_events.push_back(event);
	}

private:
	std::vector<std::weak_ptr<IEvent>> m_events;
};

template <class... Params>
class Event : public IEvent
{
private:
	using Callback = std::function<void(Params...)>;

public:
	Event() = default;
	~Event() = default;

	// 콜백 함수가 항상 안전하게 호출 될 수 있을 때만 사용
	void Register(const Callback& callback)
	{
		if (m_isOnNotifying)
			m_reserved.emplace_back(nullptr, callback);
		else
			m_observers[nullptr].push_back(callback);
	}

	void Register(IObserver* observer, const Callback& callback)
	{
		observer->Register(this->weak_from_this());
		if (m_isOnNotifying)
			m_reserved.emplace_back(observer, callback);
		else
			m_observers[observer].push_back(callback);
	}

	virtual void Unregister(IObserver* observer) override final
	{
		if (m_isOnNotifying)
			std::erase_if(m_reserved, [observer](const auto& elem) { return elem.first == observer; });
		m_observers.erase(observer);
	}

	void Notify(const Params&... params)
	{
		// Notify 도중에 Register가 호출되면 m_reserved에 넣어놓고 끝날 때 m_observers로 옮긴다.
		m_isOnNotifying = true;
		for (const auto& [_, callbacks] : m_observers)
		{
			for (const auto& callback : callbacks)
				callback(params...);
		}
		m_isOnNotifying = false;

		for (const auto& [o, c] : m_reserved)
			Register(o, c);
		m_reserved.clear();
	}

private:
	std::map<IObserver*, std::vector<Callback>> m_observers;

	bool m_isOnNotifying{ false };
	std::vector<std::pair<IObserver*, Callback>> m_reserved;
};