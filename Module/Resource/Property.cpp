#include "Stdafx.h"
#include "Include/Image.h"
#include "Include/Property.h"
#include "Include/Manager.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	bool IsSkip(std::wistream& file, std::wstring& name)
	{
		if (name.empty())
			return false;

		bool isSkip{ false };

		int32_t length{};
		file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));

		std::array<wchar_t, 128> buffer{};
		file.read(buffer.data(), length);

		// 직속 자식만 체크한다.
		std::wstring nodeName{ buffer.data(), static_cast<size_t>(length) };
		size_t pos{ name.find('/') };
		if (pos == std::wstring::npos)
			isSkip = nodeName != name;
		else
			isSkip = nodeName != name.substr(0, pos);

		if (!isSkip)
		{
			if (pos == std::wstring::npos)
				name.clear();
			else
				name = name.substr(pos + 1);
		}

		// 이 뒤에서 스트림으로부터 이름을 읽을 수 있도록 커서를 string만큼 뒤로 옮김
		file.seekg(-length - sizeof(length), std::ios::cur);

		return isSkip;
	}

	void Skip(std::wistream& file)
	{
		char length{};
		file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));
		file.ignore(length);

		auto type{ Property::Type::FOLDER };
		file.read(reinterpret_cast<wchar_t*>(&type), sizeof(type));

		switch (type)
		{
		case Property::Type::FOLDER:
			break;
		case Property::Type::INT:
			file.ignore(sizeof(int));
			break;
		case Property::Type::INT2:
			file.ignore(sizeof(INT2));
			break;
		case Property::Type::FLOAT:
			file.ignore(sizeof(float));
			break;
		case Property::Type::STRING:
		{
			char length{};
			file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));
			file.ignore(length);
			break;
		}
		case Property::Type::IMAGE:
		{
			int length{};
			file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));
			file.ignore(length);
			break;
		}
		default:
			assert(false);
			break;
		}

		int nodeCount{};
		file.read(reinterpret_cast<wchar_t*>(&nodeCount), sizeof(nodeCount));
		for (int i = 0; i < nodeCount; ++i)
			Skip(file);
	}

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
		if (m_property->m_children.empty())
			return false;
		if (m_property != it.m_property)
			return true;
		if (m_childIndex != it.m_childIndex)
			return true;
		return false;
	}

	std::pair<std::wstring, std::shared_ptr<Property>> Property::Iterator::operator*() const
	{
		return std::make_pair(
			m_property->m_children[m_childIndex]->m_name,
			m_property->m_children[m_childIndex]
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
		return Iterator{ this, m_children.size() - 1 };
	}

	void Property::Save(const std::filesystem::path& path)
	{
		std::wofstream file{ path, std::ios::binary };
		std::function<void(Property*)> lambda = [&](Property* prop)
			{
				// 이름
				std::wstring name{ prop->m_name.c_str() };
				int32_t length{ static_cast<int32_t>(name.size()) };
				file.write(reinterpret_cast<wchar_t*>(&length), sizeof(length));
				file.write(prop->m_name.data(), length * sizeof(wchar_t));

				// 타입
				file.write(reinterpret_cast<wchar_t*>(&prop->m_type), sizeof(prop->m_type));

				// 데이터
				switch (prop->m_type)
				{
				case Type::FOLDER:
					break;
				case Type::INT:
				{
					auto data{ std::get<int32_t>(prop->m_data) };
					file.write(reinterpret_cast<wchar_t*>(&data), sizeof(data));
					break;
				}
				case Type::INT2:
				{
					auto data{ std::get<INT2>(prop->m_data) };
					file.write(reinterpret_cast<wchar_t*>(&data), sizeof(data));
					break;
				}
				case Type::FLOAT:
				{
					auto data{ std::get<float>(prop->m_data) };
					file.write(reinterpret_cast<wchar_t*>(&data), sizeof(data));
					break;
				}
				case Type::STRING:
				{
					const auto& data{ std::get<std::wstring>(prop->m_data) };
					length = static_cast<int32_t>(data.size());
					file.write(reinterpret_cast<wchar_t*>(&length), sizeof(length));
					file.write(data.data(), length * sizeof(wchar_t));
					break;
				}
				case Type::IMAGE:
				{
					auto data{ std::get<std::shared_ptr<Image>>(prop->m_data) };
					auto buffer{ data->GetBuffer() };
					auto size{ static_cast<uint32_t>(buffer.size_bytes()) };
					file.write(reinterpret_cast<wchar_t*>(&size), sizeof(size));
					file.write(reinterpret_cast<wchar_t*>(buffer.data()), size);
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					break;
				}

				int32_t count{ static_cast<int32_t>(prop->m_children.size()) };
				file.write(reinterpret_cast<wchar_t*>(&count), sizeof(count));

				for (const auto& child : prop->m_children)
					lambda(child.get());
			};

		int32_t count{ static_cast<int32_t>(m_children.size()) };
		file.write(reinterpret_cast<wchar_t*>(&count), sizeof(count));

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

	std::wstring Property::GetString() const
	{
		assert(m_type == Type::STRING);
		return std::get<std::wstring>(m_data);
	}

	std::shared_ptr<Image> Property::GetImage(const std::wstring& path) const
	{
		assert(m_type == Type::IMAGE);
		if (path.empty())
			return std::get<std::shared_ptr<Image>>(m_data);
		if (auto child{ Get(path) })
			return child->GetImage();
		return nullptr;
	}

	std::shared_ptr<Property> Property::Get(const std::wstring& path) const
	{
		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
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

	void Resource::Property::Flush()
	{
		std::erase_if(m_children, [](const auto& c) { return c.use_count() <= 1; });
	}

	Property::Type GetType(const std::shared_ptr<Property>& prop)
	{
		return prop->GetType();
	}

	std::wstring GetName(const std::shared_ptr<Property>& prop)
	{
		return prop->GetName();
	}

	int GetInt(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop->GetInt();

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetInt();

		return 0;
	}

	INT2 GetInt2(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop->GetInt2();

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetInt2(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetInt2();

		return INT2{};
	}

	float GetFloat(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop->GetFloat();

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetFloat(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetFloat();

		return 0.0f;
	}

	std::wstring GetString(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop->GetString();

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
			return GetString(prop->Get(name), remain);
		}

		if (const auto & child{ prop->Get(name) })
			return child->GetString();

		return L"";
	}

	std::shared_ptr<Image> GetImage(const std::shared_ptr<Property>& prop, const std::wstring& path)
	{
		if (path.empty())
			return prop->GetImage();

		std::wstring name{ path };
		std::wstring remain{};

		size_t pos{ path.find(StringTable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
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

	std::shared_ptr<Property> Load(const std::filesystem::path& path, const std::wstring& subPath)
	{
		auto root{ std::make_shared<Property>() };
		root->SetType(Property::Type::FOLDER);
		root->SetName(L"root");

		std::wifstream file{ path, std::ios::binary };
		std::function<void(const std::shared_ptr<Property>&, std::wstring&)> lambda = [&](const std::shared_ptr<Property>& prop, std::wstring& path)
			{
				// 이름
				int32_t length{};
				file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));

				std::array<wchar_t, 128> buffer{};
				file.read(buffer.data(), length);

				std::wstring name{ buffer.data(), static_cast<size_t>(length) };
				prop->SetName(name);

				// 타입
				Property::Type type{};
				file.read(reinterpret_cast<wchar_t*>(&type), sizeof(type));
				prop->SetType(type);

				// 데이터
				switch (type)
				{
				case Property::Type::FOLDER:
					break;
				case Property::Type::INT:
				{
					int value{};
					file.read(reinterpret_cast<wchar_t*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::INT2:
				{
					INT2 value{};
					file.read(reinterpret_cast<wchar_t*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::FLOAT:
				{
					float value{};
					file.read(reinterpret_cast<wchar_t*>(&value), sizeof(value));
					prop->Set(value);
					break;
				}
				case Property::Type::STRING:
				{
					length = 0;
					file.read(reinterpret_cast<wchar_t*>(&length), sizeof(length));

					buffer.fill(0);
					file.read(buffer.data(), length * sizeof(wchar_t));

					prop->Set(std::wstring{ buffer.data(), static_cast<size_t>(length) });
					break;
				}
				case Property::Type::IMAGE:
				{
					int32_t size{};
					file.read(reinterpret_cast<wchar_t*>(&size), sizeof(size));

					std::unique_ptr<std::byte[]> buffer{ new std::byte[size]{} };
					file.read(reinterpret_cast<wchar_t*>(buffer.get()), size);

					auto image{ std::make_shared<Image>() };
					image->SetBuffer(buffer.release(), size);
					prop->Set(image);
					break;
				}
				default:
					assert(false && "INVALID PROPERTY TYPE");
					break;
				}

				// 자식
				int32_t count{};
				file.read(reinterpret_cast<wchar_t*>(&count), sizeof(count));
				for (int32_t i = 0; i < count; ++i)
				{
					std::wstring temp{ path };
					if (IsSkip(file, temp))
					{
						Skip(file);
						continue;
					}

					auto child{ std::make_shared<Property>() };
					lambda(child, temp);
					prop->Add(child);
				}
			};

		return root;
	}
}