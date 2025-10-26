#pragma once

class IDelegate
{
public:
	struct Handle
	{
		std::weak_ptr<IDelegate> owner;
		std::uint64_t id;
	};

	class Listener
	{
		template<class... Params>
		friend class Delegate;

	public:
		~Listener()
		{
			for (const auto& handle : m_handles)
			{
				if (auto owner{ handle.owner.lock() })
					owner->Unbind(handle);
			}
		}

	private:
		std::vector<Handle> m_handles;
	};

public:
	virtual ~IDelegate() {}
	virtual void Unbind() {}
	virtual void Unbind(const Handle& handle) {}
};

template <class... Params>
class Delegate
{
public:
	using Callback = std::function<void(Params...)>;

private:
	class Impl :
		public IDelegate,
		public std::enable_shared_from_this<IDelegate>
	{
	private:
		struct Entry
		{
			Handle handle;
			Callback callback;
		};

	public:
		Impl() :
			m_nextHandleID{ 0 }
		{
		}

		~Impl() override
		{
			Unbind();
		}

		Handle Bind(const Callback& callback)
		{
			Handle handle{ weak_from_this(), m_nextHandleID++ };
			m_entries.emplace_back(handle, callback);
			return handle;
		}

		void Bind(IDelegate::Listener* listener, const Callback& callback)
		{
			auto handle{ Bind(callback) };
			listener->m_handles.push_back(handle);
		}

		void Unbind() override
		{
			m_entries.clear();
		}

		void Unbind(const Handle& handle) override
		{
			auto owner{ handle.owner.lock() };
			if (owner.get() != this)
				return;

			auto it{ std::ranges::find(m_entries, handle.id, [](const auto& entry) { return entry.handle.id; }) };
			if (it != m_entries.end())
				m_entries.erase(it);
		}

		void Broadcast(const Params&... params) const
		{
			for (const auto& entry : m_entries)
				entry.callback(params...);
		}

	private:
		std::uint64_t m_nextHandleID;
		std::vector<Entry> m_entries;
	};

public:
	Delegate() :
		m_impl{ std::make_shared<Impl>() }
	{
	}

	IDelegate::Handle Bind(const Callback& callback)
	{
		return m_impl->Bind(callback);
	}

	void Bind(IDelegate::Listener* listener, const Callback& callback)
	{
		m_impl->Bind(listener, callback);
	}

	void Unbind()
	{
		m_impl->Unbind();
	}

	void Unbind(const IDelegate::Handle& handle)
	{
		m_impl->Unbind(handle);
	}

	void Broadcast(const Params&... params) const
	{
		m_impl->Broadcast(params...);
	}

private:
	std::shared_ptr<Impl> m_impl;
};
