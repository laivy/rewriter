#include "Stdafx.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/ResourceManager.h"
#include "External/DirectX/WICTextureLoader12.h"
#include <functional>

namespace Resource
{
	Property::Iterator::Iterator(const Property* const p, size_t index) :
		m_property{ p },
		m_childIndex{ index }
	{
	}

	Property::Iterator& Property::Iterator::operator++()
	{
		++m_childIndex;
		return *this;
	}

	Property::Iterator& Property::Iterator::operator--()
	{
		--m_childIndex;
		return *this;
	}

	bool Property::Iterator::operator!=(const Iterator& it) const
	{
		if (m_property != it.m_property)
			return true;
		if (m_childIndex != it.m_childIndex)
			return true;
		return false;
	}

	std::pair<std::string, std::shared_ptr<Property>> Property::Iterator::operator*() const
	{
		return std::make_pair(
			m_property->m_children[m_childIndex]->m_name,
			m_property->m_children[m_childIndex]
		);
	}

	Property::Property() :
		m_type{ Property::Type::FOLDER },
		m_name{ "" }
	{
	}

	Property::~Property()
	{
	}

	Property::Iterator Property::begin() const
	{
		return Iterator{ this, 0 };
	}

	Property::Iterator Property::end() const
	{
		return Iterator{ this, m_children.size() - 1 };
	}

	void Property::Save(std::ostream& file)
	{
		std::function<void(Property*)> lambda = [&](Property* prop)
			{
				// 이름
				int32_t length{ static_cast<int32_t>(prop->m_name.size()) };
				file.write(reinterpret_cast<char*>(&length), sizeof(length));
				file.write(prop->m_name.data(), sizeof(length));

				// 타입
				file.write(reinterpret_cast<char*>(prop->m_type), sizeof(prop->m_type));

				// 데이터
				switch (prop->m_type)
				{
				case Type::INT:
				{
					auto data{ std::get<int32_t>(prop->m_data) };
					file.write(reinterpret_cast<char*>(&data), sizeof(data));
					break;
				}
				case Type::INT2:
				{
					auto data{ std::get<INT2>(prop->m_data) };
					file.write(reinterpret_cast<char*>(&data), sizeof(data));
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
					auto data{ std::get<std::string>(prop->m_data) };
					length = static_cast<int32_t>(data.size());
					file.write(reinterpret_cast<char*>(&length), sizeof(length));
					file.write(data.data(), sizeof(length));
					break;
				}
				case Type::IMAGE:
				{
					auto data{ std::get<std::shared_ptr<Image>>(prop->m_data) };
					auto [binary, size] { data->GetBinary() };
					file.write(reinterpret_cast<char*>(&size), sizeof(size));
					file.write(reinterpret_cast<char*>(binary), size);
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					break;
				}

				for (const auto& child : prop->m_children)
					lambda(child.get());
			};

		int32_t count{ static_cast<int32_t>(m_children.size()) };
		file.write(reinterpret_cast<char*>(&count), sizeof(count));

		for (const auto& child : m_children)
			lambda(child.get());
	}

	void Property::Load(std::istream& file)
	{
		std::string dummy{};

		uint32_t count{};
		file.read(reinterpret_cast<char*>(&count), sizeof(count));

		for (uint32_t i = 0; i < count; ++i)
		{
			m_children.emplace_back(new Property);
			m_children.back()->Load(file, dummy);
		}
	}

	void Property::SetType(Type type)
	{
		m_type = type;
	}

	void Property::SetName(const std::string& name)
	{
		m_name = name;
	}

	void Property::Set(int data)
	{
		m_data = data;
	}

	void Property::Set(const INT2& data)
	{
		m_data = data;
	}

	void Property::Set(float data)
	{
		m_data = data;
	}

	void Property::Set(const std::string& data)
	{
		m_data = data;
	}

	void Property::Set(const std::shared_ptr<Image>& data)
	{
		m_data = data;
	}

	Property::Type Property::GetType() const
	{
		return m_type;
	}

	std::string Property::GetName() const
	{
		return m_name;
	}

	int Property::GetInt() const
	{
		assert(m_type == Type::INT);
		return std::get<int>(m_data);
	}

	INT2 Property::GetInt2() const
	{
		assert(m_type == Type::INT2);
		return std::get<INT2>(m_data);
	}

	float Property::GetFloat() const
	{
		assert(m_type == Type::FLOAT);
		return std::get<float>(m_data);
	}

	std::string Property::GetString() const
	{
		assert(m_type == Type::STRING);
		return std::get<std::string>(m_data);
	}

	std::shared_ptr<Image> Property::GetImage() const
	{
		assert(m_type == Type::IMAGE);
		return std::get<std::shared_ptr<Image>>(m_data);
	}

	std::shared_ptr<Property> Property::Get(const std::string& path) const
	{
		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			std::string childName{ path.substr(0, pos) };
			std::string remain{ path.substr(pos + 1) };
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

	void Property::Load(std::istream& file, std::string& name)
	{
		// 이름
		char length{};
		file.read(reinterpret_cast<char*>(&length), sizeof(length));

		std::array<char, 128> buffer{};
		file.read(buffer.data(), length);

		m_name.assign(buffer.data(), length);

		// 타입
		file.read(reinterpret_cast<char*>(&m_type), sizeof(m_type));

		// 데이터
		switch (m_type)
		{
		case Type::FOLDER:
			break;
		case Type::INT:
		{
			int data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			m_data = data;
			break;
		}
		case Type::INT2:
		{
			INT2 data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			m_data = data;
			break;
		}
		case Type::FLOAT:
		{
			float data{};
			file.read(reinterpret_cast<char*>(&data), sizeof(data));
			m_data = data;
			break;
		}
		case Type::STRING:
		{
			length = 0;
			file.read(reinterpret_cast<char*>(&length), sizeof(length));

			buffer.fill(0);
			file.read(buffer.data(), length);

			m_data = std::string{ buffer.data(), static_cast<size_t>(length) };
			break;
		}
		case Type::IMAGE:
		{
			int len{};
			file.read(reinterpret_cast<char*>(&len), sizeof(len));

			std::unique_ptr<BYTE[]> binary{ new BYTE[len]{} };
			file.read(reinterpret_cast<char*>(binary.get()), len);

			m_data = std::make_unique<Image>(binary.get(), static_cast<DWORD>(len));
			break;
		}
		default:
			assert(false);
			break;
		}

		// 자식 노드
		uint32_t nodeCount{};
		file.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

		if (nodeCount > 0)
		{
			m_children.reserve(nodeCount);
			for (uint32_t i = 0; i < nodeCount; ++i)
			{
				std::string temp{ name };
				if (IsSkip(file, temp))
				{
					Skip(file);
					continue;
				}
				m_children.emplace_back(new Property);
				m_children.back()->Load(file, temp);
			}
		}
	}

	void Resource::Property::Flush()
	{
		std::erase_if(m_children, [](const auto& c) { return c.use_count() <= 1; });
	}

	Property::Type GetType(const std::shared_ptr<Property>& prop)
	{
		return prop->GetType();
	}

	std::string GetName(const std::shared_ptr<Property>& prop)
	{
		return prop->GetName();
	}

	int GetInt(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetInt();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetInt();

		return 0;
	}

	INT2 GetInt2(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetInt2();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt2(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetInt2();

		return INT2{};
	}

	float GetFloat(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetFloat();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetFloat(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetFloat();

		return 0.0f;
	}

	std::string GetString(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetString();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetString(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetString();

		return "";
	}

	std::shared_ptr<Image> GetImage(const std::shared_ptr<Property>& prop, const std::string& path)
	{
		if (path.empty())
			return prop->GetImage();

		std::string name{ path };
		std::string remain{};

		size_t pos{ path.find('/') };
		if (pos != std::string::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetImage(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetImage();

		return nullptr;
	}

	std::shared_ptr<Property> Create()
	{
		return std::make_shared<Property>();
	}

	void SetType(const std::shared_ptr<Property>& prop, Property::Type type)
	{
		prop->SetType(type);
	}

	void SetName(const std::shared_ptr<Property>& prop, const std::string& name)
	{
		prop->SetName(name);
	}

	void Set(const std::shared_ptr<Property>& prop, int32_t value)
	{
		prop->Set(value);
	}

	void Set(const std::shared_ptr<Property>& prop, const INT2& value)
	{
		prop->Set(value);
	}

	void Set(const std::shared_ptr<Property>&prop, float value)
	{
		prop->Set(value);
	}

	void Set(const std::shared_ptr<Property>&prop, const std::string& value)
	{
		prop->Set(value);
	}

	void Set(const std::shared_ptr<Property>&prop, const std::shared_ptr<Image>& value)
	{
		prop->Set(value);
	}

#ifdef _TOOL
	void Save(const std::shared_ptr<Property>& prop, std::ostream& file)
	{
		prop->Save(file);
	}

	std::shared_ptr<Property> Load(std::istream& file)
	{
		auto root{ std::make_shared<Property>() };
		root->SetType(Property::Type::FOLDER);
		root->SetName("root");
		root->Load(file);
		return root;
	}
#endif
}