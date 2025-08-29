#pragma once

// Delegate에 콜백 함수를 등록할 개체의 수명이 무한하지 않을 경우 이 클래스를 상속받는 것이 좋다.
class IObserver
{
	template <class... Params>
	friend class Delegate;

private:
	// Delegate에서 참조할 weak_ptr를 만들기 위한 더미 타입
	class Reference	{};

public:
	IObserver() : m_reference{ new Reference }
	{
	}

private:
	std::shared_ptr<Reference> m_reference;
};

template <class... Params>
class Delegate
{
private:
	using Callback = std::function<void(Params...)>;

public:
	void Register(const Callback& callback)
	{
		m_rawCallbacks.push_back(callback);
	}

	void Register(IObserver* observer, const Callback& callback)
	{
		m_safeCallbacks.emplace_back(observer->m_reference, callback);
	}

	void Notify(const Params&... params)
	{
		for (const auto& callback : m_rawCallbacks)
			callback(params...);

		std::erase_if(m_safeCallbacks, [&params...](const auto& elem)
		{
			const auto& [ref, callback] { elem };
			auto lock{ ref.lock() };
			if (!lock)
				return true;

			callback(params...);
			return false;
		});
	}

private:
	std::vector<Callback> m_rawCallbacks;
	std::vector<std::pair<std::weak_ptr<IObserver::Reference>, Callback>> m_safeCallbacks;
};
