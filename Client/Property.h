#pragma once

class Property
{
public:
	friend class ResourceManager;

	// NytTreeNodeInfo.cs에 정의되어 있는 NytDataType와 동일해야함
	enum class Type
	{
		GROUP,
		INT,
		INT2,
		FLOAT,
		STRING,
		D2DImage,
		D3DImage
	};

	typedef std::variant<INT, INT2, FLOAT, std::string, Image> Data;

public:
	Property();
	Property(Type type, const Data& data);
	~Property() = default;

	auto begin() { return m_childProps.begin(); }
	auto end() { return m_childProps.end(); }

	template<class T>
	T* Get()
	{
		return &std::get<T>(m_data);
	}

	template<>
	Property* Get()
	{
		return this;
	}

	template<class T>
	T* Get(const std::string& name)
	{
		// 하위 프로퍼티에서 가져옴
		size_t pos{ name.find('/') };
		if (pos != std::string::npos)
		{
			std::string childName{ name.substr(0, pos) };
			if (m_childProps.contains(childName))
				return m_childProps.at(childName)->Get<T>(name.substr(pos + 1));
			return nullptr;
		}

		// 해당 이름의 자식 프로퍼티가 있는지 확인
		if (!m_childProps.contains(name))
			return nullptr;

		if constexpr (std::is_same_v<T, Property>)
			return m_childProps.at(name).get();

		return m_childProps.at(name)->Get<T>();
	}

	size_t GetChildCount() const
	{
		return m_childProps.size();
	}

private:
	Type m_type;
	Data m_data;
	std::unordered_map<std::string, std::unique_ptr<Property>> m_childProps;
};