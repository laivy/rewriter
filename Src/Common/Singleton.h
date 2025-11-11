#pragma once

template <class T>
class Singleton
{
public:
	template <class... Args>
	static T* Instantiate(Args&&... args)
	{
		if (!m_instance)
			m_instance = std::make_unique<T>(std::forward<Args>(args)...);
		return m_instance.get();
	}

	static void Destroy()
	{
		m_instance.reset();
	}

	static T* GetInstance()
	{
		return m_instance.get();
	}

	static bool IsInstanced()
	{
		return m_instance ? true : false;
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

private:
	static inline std::unique_ptr<T> m_instance;
};
