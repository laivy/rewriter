#pragma once

class Property
{
public:
	friend class ResourceManager;

	// NytTreeNodeInfo.cs에 정의되어 있는 NytDataType와 동일해야함
	enum class Type
	{
		GROUP, INT, INT2, FLOAT, STRING, D2DImage, D3DImage
	};

	typedef std::variant<INT, INT2, FLOAT, std::string, Image> Data;

public:
	Property();
	Property(Type type, const Data& data);
	~Property() = default;

	auto begin() { return m_child.begin(); }
	auto end() { return m_child.end(); }

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
		std::string smallCaseName{ name };
		if (std::ranges::any_of(smallCaseName, [](unsigned char c) { return std::isupper(c); }))
			std::ranges::transform(smallCaseName, smallCaseName.begin(), [](unsigned char c) { return std::tolower(c); });

		// 하위 프로퍼티에서 가져옴
		size_t pos{ smallCaseName.find('/') };
		if (pos != std::string::npos)
		{
			std::string childName{ smallCaseName.substr(0, pos) };
			if (m_child.contains(childName))
				return m_child.at(childName)->Get<T>(smallCaseName.substr(pos + 1));
			return nullptr;
		}

		// 해당 이름의 자식 프로퍼티가 있는지 확인
		if (!m_child.contains(smallCaseName))
			return nullptr;

		if constexpr (std::is_same_v<T, Property>)
			return m_child.at(smallCaseName).get();

		return m_child.at(smallCaseName)->Get<T>();
	}

	size_t GetChildCount() const
	{
		return m_child.size();
	}

private:
	Type m_type;
	Data m_data;
	std::unordered_map<std::string, std::unique_ptr<Property>> m_child;
};