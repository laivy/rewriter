#include "Stdafx.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/Manager.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	Property::Iterator::Iterator(const Property* const prop, size_t index) :
		m_property{ prop },
		m_index{ index }
	{
	}

	Property::Iterator& Property::Iterator::operator++()
	{
		++m_index;
		return *this;
	}

	Property::Iterator& Property::Iterator::operator--()
	{
		--m_index;
		return *this;
	}

	bool Property::Iterator::operator!=(const Iterator& it) const
	{
		if (m_property->m_children.empty())
			return false;
		if (m_property != it.m_property)
			return true;
		if (m_index != it.m_index)
			return true;
		return false;
	}

	std::pair<std::wstring, std::shared_ptr<Property>> Property::Iterator::operator*() const
	{
		return std::make_pair(
			m_property->m_children[m_index]->m_name,
			m_property->m_children[m_index]
		);
	}

	Property::Property() :
		m_type{ Property::Type::FOLDER },
		m_name{ L"" }
	{
	}

	Property::Iterator Property::begin() const
	{
		return Iterator{ this, 0 };
	}

	Property::Iterator Property::end() const
	{
		return Iterator{ this, m_children.size() };
	}

	void Property::Save(const std::filesystem::path& path)
	{
		std::ofstream file{ path, std::ios::binary };
		std::function<void(Property*)> lambda = [&](Property* prop)
			{
				// 이름
				std::wstring name{ prop->m_name.c_str() };
				int32_t length{ static_cast<int32_t>(name.size()) };
				file.write(reinterpret_cast<char*>(&length), sizeof(length));
				file.write(reinterpret_cast<char*>(prop->m_name.data()), length * sizeof(wchar_t));

				// 타입
				file.write(reinterpret_cast<char*>(&prop->m_type), sizeof(prop->m_type));

				// 데이터
				switch (prop->m_type)
				{
				case Type::FOLDER:
					break;
				case Type::INT:
				{
					auto data{ std::get<int32_t>(prop->m_data) };
					file.write(reinterpret_cast<char*>(&data), sizeof(data));
					break;
				}
				case Type::INT2:
				{
					const auto& data{ std::get<INT2>(prop->m_data) };
					file.write(reinterpret_cast<const char*>(&data), sizeof(data));
					break;
				}
				case Type::FLOAT:
				{
					auto data{ std::get<float>(prop->m_data) };
					file.write(reinterpret_cast<char*>(&data), sizeof(data));
					break;
				}
				case Type::STRING:
				{
					const auto& data{ std::get<std::wstring>(prop->m_data) };
					length = static_cast<int32_t>(data.size());
					file.write(reinterpret_cast<char*>(&length), sizeof(length));
					file.write(reinterpret_cast<const char*>(data.data()), length * sizeof(wchar_t));
					break;
				}
				case Type::IMAGE:
				{
					auto data{ std::get<std::shared_ptr<Image>>(prop->m_data) };
					auto buffer{ data->GetBuffer() };
					auto size{ static_cast<uint32_t>(buffer.size_bytes()) };
					file.write(reinterpret_cast<char*>(&size), sizeof(size));
					file.write(reinterpret_cast<char*>(buffer.data()), size);
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					break;
				}

				int32_t count{ static_cast<int32_t>(prop->m_children.size()) };
				file.write(reinterpret_cast<char*>(&count), sizeof(count));

				for (const auto& child : prop->m_children)
					lambda(child.get());
			};

		int32_t count{ static_cast<int32_t>(m_children.size()) };
		file.write(reinterpret_cast<char*>(&count), sizeof(count));

		for (const auto& child : m_children)
			lambda(child.get());
	}

	void Property::Add(const std::shared_ptr<Property>& child)
	{
		m_children.push_back(child);
	}

	void Property::SetType(Type type)
	{
		m_type = type;
	}

	void Property::SetName(const std::wstring& name)
	{
		m_name = name;
	}

	void Property::Set(int value)
	{
		m_data = value;
	}

	void Property::Set(const INT2& value)
	{
		m_data = value;
	}

	void Property::Set(float value)
	{
		m_data = value;
	}

	void Property::Set(const std::wstring& value)
	{
		m_data = value;
	}

	void Property::Set(const std::shared_ptr<Image>& value)
	{
		m_data = value;
	}

	Property::Type Property::GetType() const
	{
		return m_type;
	}

	std::wstring Property::GetName() const
	{
		return m_name;
	}

	int Property::GetInt(const std::wstring& path) const
	{
		assert(m_type == Type::INT);
		
		if (path.empty())
			return std::get<int>(m_data);

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto& child{ Get(name) })
			return child->GetInt(remain);

		return 0;
	}

	INT2 Property::GetInt2(const std::wstring& path) const
	{
		assert(m_type == Type::INT2);

		if (path.empty())
			return std::get<INT2>(m_data);

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto& child{ Get(name) })
			return child->GetInt2(remain);

		return INT2{};
	}

	float Property::GetFloat(const std::wstring& path) const
	{
		assert(m_type == Type::FLOAT);

		if (path.empty())
			return std::get<float>(m_data);

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto& child{ Get(name) })
			return child->GetFloat(remain);

		return 0.0f;
	}

	std::wstring Property::GetString(const std::wstring& path) const
	{
		assert(m_type == Type::STRING);

		if (path.empty())
			return std::get<std::wstring>(m_data);

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetString(remain);

		return L"";
	}

	std::shared_ptr<Image> Property::GetImage(const std::wstring& path) const
	{
		assert(m_type == Type::IMAGE);

		if (path.empty())
			return std::get<std::shared_ptr<Image>>(m_data);

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetImage(remain);

		return nullptr;
	}

	std::shared_ptr<Property> Property::Get(const std::wstring& path) const
	{
		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			std::wstring childName{ path.substr(0, pos) };
			std::wstring remain{ path.substr(pos + 1) };
			auto it{ std::ranges::find_if(m_children, [&](const auto& p) { return p->m_name == childName; }) };
			if (it == m_children.end())
				return nullptr;
			return (*it)->Get(remain);
		}

		auto it{ std::ranges::find_if(m_children, [&](const auto& p) { return p->m_name == path; }) };
		if (it == m_children.end())
			return nullptr;
		return *it;
	}

	void Property::Flush()
	{
		std::erase_if(m_children, [](const auto& c) { return c.use_count() <= 1; });
	}
}