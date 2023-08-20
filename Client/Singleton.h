#pragma once

template <class T>
class TSingleton
{
public:
	template <class... Args>
	static void Instantiate(Args&&... args)
	{
		if (m_instance) return;
		m_instance = std::make_unique<T>(std::forward<Args>(args)...);
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
		return m_instance.get() == nullptr ? FALSE : TRUE;
	}

protected:
	TSingleton() = default;
	virtual ~TSingleton() = default;

private:
	static std::unique_ptr<T> m_instance;
};

template <class T>
std::unique_ptr<T> TSingleton<T>::m_instance{ nullptr };