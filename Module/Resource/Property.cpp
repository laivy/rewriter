#include "Stdafx.h"
#include "PNG.h"
#include "Property.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	DLL_API Property::Property() :
		m_type{ Property::Type::Folder },
		m_name{ L"" }
	{
	}

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
		case Type::Int:
			m_data = 0;
			break;
		case Type::Int2:
			m_data = INT2{ 0, 0 };
			break;
		case Type::Float:
			m_data = 0.0f;
			break;
		case Type::String:
			m_data = L"";
			break;
		case Type::Image:
			m_data = std::shared_ptr<Resource::PNG>{};
			break;
		}
	}

	DLL_API void Property::SetName(const std::wstring& name)
	{
		this->m_name = name;
	}

	DLL_API void Property::Set(int32_t value)
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

	DLL_API int32_t Property::GetInt(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::Int);
			return std::get<int32_t>(m_data);
		}

		std::wstring_view name{ path };
		std::wstring_view remain{};

		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			name = path.substr(0, pos);
			remain = path.substr(pos + 1);
		}

		if (const auto& child{ Get(name) })
			return child->GetInt(remain);

		return 0;
	}

	DLL_API INT2 Property::GetInt2(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::Int2);
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

		if (const auto& child{ Get(name) })
			return child->GetInt2(remain);

		return INT2{};
	}

	DLL_API float Property::GetFloat(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::Float);
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

		if (const auto& child{ Get(name) })
			return child->GetFloat(remain);

		return 0.0f;
	}

	DLL_API std::wstring Property::GetString(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::String);
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

		if (const auto& child{ Get(name) })
			return child->GetString(remain);

		return L"";
	}

	DLL_API std::shared_ptr<PNG> Property::GetImage(std::wstring_view path) const
	{
		if (path.empty())
		{
			assert(m_type == Type::Image);
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

		if (const auto& child{ Get(name) })
			return child->GetImage(remain);

		assert(m_type == Type::Image);
		return nullptr;
	}

	DLL_API std::shared_ptr<Property> Property::Get(std::wstring_view path) const
	{
		size_t pos{ path.find(Stringtable::DATA_PATH_SEPERATOR) };
		if (pos != std::wstring_view::npos)
		{
			auto childName{ path.substr(0, pos) };
			auto remain{ path.substr(pos + 1) };
			auto it{ std::ranges::find_if(m_children, [childName](const auto& p) { return p->m_name == childName; }) };
			if (it == m_children.end())
				return nullptr;
			return (*it)->Get(remain);
		}

		auto it{ std::ranges::find_if(m_children, [path](const auto& p) { return p->m_name == path; }) };
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
