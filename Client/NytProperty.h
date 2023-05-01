#pragma once

class NytProperty
{
public:
	friend class ResourceManager;

	auto begin() { return m_childProps.begin(); }
	auto end() { return m_childProps.end(); }

public:
	NytProperty();
	NytProperty(NytType type, const std::any& data);
	~NytProperty();

	template<class T>
	T* Get() const
	{
		return std::any_cast<T*>(m_data);
	}

	template<class T>
	T* Get(const std::string& name) const
	{
		// 하위 프로퍼티에서 가져옴
		size_t pos{ name.find('/') };
		if (pos != std::string::npos)
		{
			std::string childName{ name.substr(0, pos) };
			if (m_childProps.contains(childName))
				return m_childProps.at(childName)->Get<T>(name.substr(pos + 1));
			assert(false);
		}

		// 해당 이름의 자식 프로퍼티가 있는지 확인
		if (!m_childProps.contains(name))
			assert(false);

		if constexpr (std::is_same_v<T, NytProperty>)
			return m_childProps.at(name).get();

		return m_childProps.at(name)->Get<T>();
	}

	size_t GetChildCount() const
	{
		return m_childProps.size();
	}

private:
	NytType m_type;
	std::any m_data;
	std::unordered_map<std::string, std::unique_ptr<NytProperty>> m_childProps;
};