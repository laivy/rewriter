#include "Stdafx.h"
#include "PNG.h"
#include "Property.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	DLL_API Property::Property() :
		m_type{ Property::Type::FOLDER },
		m_name{ L"" }
	{
	}

#ifdef _TOOL
	DLL_API void Property::Save(const std::filesystem::path& path)
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
				case Type::PNG:
				{
					auto data{ std::get<std::shared_ptr<PNG>>(prop->m_data) };
					auto buffer{ data->GetBinary() };
					auto size{ data->GetBinarySize() };
					file.write(reinterpret_cast<char*>(&size), sizeof(size));
					file.write(reinterpret_cast<char*>(buffer), size);
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
#endif

	DLL_API void Property::Add(const std::shared_ptr<Property>& child)
	{
		m_children.push_back(child);
	}

	DLL_API void Property::Delete(const std::shared_ptr<Property>& child)
	{
		std::erase(m_children, child);
	}

	DLL_API void Property::SetType(Type type)
	{
		m_type = type;
		switch (m_type)
		{
		case Type::INT:
			m_data = 0;
			break;
		case Type::INT2:
			m_data = INT2{ 0, 0 };
			break;
		case Type::FLOAT:
			m_data = 0.0f;
			break;
		case Type::STRING:
			m_data = L"";
			break;
		case Type::PNG:
			m_data = std::shared_ptr<Resource::PNG>{};
			break;
		}
	}

	DLL_API void Property::SetName(const std::wstring& name)
	{
		this->m_name = name;
	}

	DLL_API void Property::Set(int value)
	{
		m_data = value;
	}

	DLL_API void Property::Set(const INT2& value)
	{
		m_data = value;
	}

	DLL_API void Property::Set(float value)
	{
		m_data = value;
	}

	DLL_API void Property::Set(const std::wstring& value)
	{
		m_data = value;
	}

	DLL_API void Property::Set(const std::shared_ptr<PNG>& value)
	{
		m_data = value;
	}

	DLL_API Property::Type Property::GetType() const
	{
		return m_type;
	}

	DLL_API std::wstring Property::GetName() const
	{
		return m_name;
	}

	DLL_API int Property::GetInt(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::INT);
			return std::get<int>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetInt(remain);

		return 0;
	}

	DLL_API INT2 Property::GetInt2(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::INT2);
			return std::get<INT2>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetInt2(remain);

		return INT2{};
	}

	DLL_API float Property::GetFloat(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::FLOAT);
			return std::get<float>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetFloat(remain);

		return 0.0f;
	}

	DLL_API std::wstring Property::GetString(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::STRING);
			return std::get<std::wstring>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetString(remain);

		return L"";
	}

	DLL_API std::shared_ptr<PNG> Property::GetImage(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::PNG);
			return std::get<std::shared_ptr<PNG>>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto & child{ Get(name) })
			return child->GetImage(remain);

		assert(m_type == Type::PNG);
		return nullptr;
	}

	DLL_API std::shared_ptr<Property> Property::Get(std::wstring_view path) const
	{
		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring::npos)
		{
			std::wstring_view childName{ path.substr(0, pos) };
			std::wstring_view remain{ path.substr(pos + 1) };
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

	DLL_API const std::vector<std::shared_ptr<Property>>& Property::GetChildren() const
	{
		return m_children;
	}

	DLL_API std::vector<std::shared_ptr<Property>>& Property::GetChildren()
	{
		return m_children;
	}
}