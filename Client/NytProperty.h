#pragma once

class NytProperty
{
public:
	friend class NytLoader;

	class iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;

		iterator(const NytProperty& prop, size_t index = 0) : m_root{ prop }, m_index{ index } { }

		iterator& operator++()
		{
			++m_index;
			return *this;
		}

		std::pair<std::string, NytProperty*> operator*() const
		{
			return std::make_pair(
				m_root.m_childNames[m_index],
				m_root.m_childProps.at(m_root.m_childNames[m_index]).get()
			);
		}

		bool operator!=(const iterator& iter) const
		{
			if (&m_root != &iter.m_root)
				return true;
			if (m_index != iter.m_index)
				return true;
			return false;
		}

	private:
		const NytProperty& m_root;
		size_t m_index;
	};

	iterator begin() { return iterator{ *this }; }
	iterator end() { return iterator{ *this, m_childNames.size() }; }
	iterator begin() const { return iterator{ *this }; }
	iterator end() const { return iterator{ *this, m_childNames.size() }; }

public:
	NytProperty();
	NytProperty(NytDataType type, const std::any& data);
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

private:
	NytDataType m_type;
	std::any m_data;

	std::vector<std::string> m_childNames;
	std::unordered_map<std::string, std::unique_ptr<NytProperty>> m_childProps;
};