#include "Stdafx.h"
#include <variant>
#include "Global.h"
#include "Model.h"
#include "Property.h"
#include "Sprite.h"
#include "External/DirectX/WICTextureLoader12.h"

namespace Resource
{
	struct Property
	{
		std::wstring name;
		std::variant<
			std::int32_t,
			Int2,
			float,
			std::wstring,
			std::shared_ptr<Sprite>,
			std::shared_ptr<Model>
		> data;
		std::vector<std::shared_ptr<Property>> children;

#ifdef _TOOL
		Type type{ Type::Folder };
		std::weak_ptr<Property> parent;
#endif
	};

	Iterator::Iterator() :
		m_prop{ nullptr }
	{
	}

	Iterator::Iterator(std::wstring_view path) :
		Iterator{ Get(path) }
	{
	}

	Iterator::Iterator(const std::shared_ptr<Property>& prop) :
		m_prop{ prop.get() }
	{
	}

	Iterator::reference Iterator::operator*() const
	{
		auto& prop{ *m_iter };
		return std::make_pair(prop->name, std::ref(prop));
	}

	Iterator& Iterator::operator++()
	{
		++m_iter;
		return *this;
	}

	Iterator Iterator::operator++(int)
	{
		Iterator temp{ *this };
		++*this;
		return temp;
	}

	Iterator& Iterator::operator+=(const difference_type offset)
	{
		m_iter += offset;
		return *this;
	}

	Iterator Iterator::operator+(const difference_type offset)
	{
		Iterator temp{ *this };
		temp += offset;
		return temp;
	}

	Iterator& Iterator::operator--()
	{
		--m_iter;
		return *this;
	}

	Iterator Iterator::operator--(int)
	{
		Iterator temp{ *this };
		--*this;
		return temp;
	}

	Iterator& Iterator::operator-=(const difference_type offset)
	{
		m_iter -= offset;
		return *this;
	}

	Iterator Iterator::operator-(const difference_type offset)
	{
		Iterator temp{ *this };
		temp -= offset;
		return temp;
	}

	Iterator::reference Iterator::operator[](const difference_type offset)
	{
		return *(*this + offset);
	}

	bool Iterator::operator==(const Iterator& rhs) const
	{
		return m_iter == rhs.m_iter;
	}

	bool Iterator::operator!=(const Iterator& rhs) const
	{
		return m_iter != rhs.m_iter;
	}

	Iterator Iterator::begin() const
	{
		if (!m_prop)
			return Iterator{};

		Iterator iter{};
		iter.m_prop = m_prop;
		iter.m_iter = m_prop->children.begin();
		return iter;
	}

	Iterator Iterator::end() const
	{
		if (!m_prop)
			return Iterator{};

		Iterator iter{};
		iter.m_prop = m_prop;
		iter.m_iter = m_prop->children.end();
		return iter;
	}

	RecursiveIterator::RecursiveIterator() :
		m_prop{ nullptr }
	{
	}

	RecursiveIterator::RecursiveIterator(std::wstring_view path) :
		RecursiveIterator{ Get(path) }
	{
	}

	RecursiveIterator::RecursiveIterator(const std::shared_ptr<Property>& prop) :
		m_prop{ prop.get() }
	{
	}

	RecursiveIterator& RecursiveIterator::operator++()
	{
		auto& p{ *m_iter };
		if (p->children.empty())
		{
			++m_iter;
			if (!m_ends.empty() && m_iter == m_ends.back())
			{
				if (!m_parents.empty())
				{
					m_iter = m_parents.back() + 1;
					m_parents.pop_back();
				}
				m_ends.pop_back();
			}
		}
		else
		{
			m_parents.push_back(m_iter);
			m_iter = p->children.begin();
			m_ends.push_back(p->children.end());
		}
		return *this;
	}

	RecursiveIterator RecursiveIterator::operator++(int)
	{
		RecursiveIterator temp{ *this };
		++*this;
		return temp;
	}

	bool RecursiveIterator::operator==(const RecursiveIterator& rhs) const
	{
		if (m_parents != rhs.m_parents)
			return false;
		if (m_iter != rhs.m_iter)
			return false;
		return true;
	}

	bool RecursiveIterator::operator!=(const RecursiveIterator& rhs) const
	{
		if (m_parents != rhs.m_parents)
			return true;
		if (m_iter != rhs.m_iter)
			return true;
		return false;
	}

	RecursiveIterator::reference RecursiveIterator::operator*() const
	{
		std::wstring fullName;
		for (const auto& name : m_parents
							  | std::ranges::views::transform([](const auto& parent) { return (*parent)->name; })
							  | std::ranges::views::join_with('/'))
		{
			fullName += name;
		}

		auto& p{ *m_iter };
		if (fullName.empty())
			fullName = p->name;
		else
			fullName += L"/" + p->name;
		return std::make_pair(fullName, std::ref(p));
	}

	RecursiveIterator RecursiveIterator::begin() const
	{
		if (!m_prop)
			return RecursiveIterator{};

		RecursiveIterator iter{};
		iter.m_prop = m_prop;
		iter.m_iter = m_prop->children.begin();
		return iter;
	}

	RecursiveIterator RecursiveIterator::end() const
	{
		if (!m_prop)
			return RecursiveIterator{};

		RecursiveIterator iter{};
		iter.m_prop = m_prop;
		iter.m_iter = m_prop->children.end();
		return iter;
	}
}

namespace
{
	std::shared_ptr<Resource::Property> Load(const std::filesystem::path& filePath, std::wstring_view subPath)
	{
		std::ifstream file;
		if (filePath.is_absolute())
			file.open(filePath, std::ios::binary);
		else
			file.open(g_mountPath / filePath, std::ios::binary);

		if (!file)
			return nullptr;

		auto recursive = [&file](this const auto& self) -> std::shared_ptr<Resource::Property>
		{
			auto prop{ std::make_shared<Resource::Property>() };

			// 이름
			std::uint16_t length{};
			file.read(reinterpret_cast<char*>(&length), sizeof(length));
			prop->name.resize(length);
			file.read(reinterpret_cast<char*>(prop->name.data()), length * sizeof(std::wstring::value_type));

			// 타입
			file.read(reinterpret_cast<char*>(&prop->type), sizeof(prop->type));

			// 데이터
			switch (prop->type)
			{
			case Resource::Type::Folder:
			{
				break;
			}
			case Resource::Type::Int:
			{
				std::int32_t data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Type::Int2:
			{
				Int2 data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Type::Float:
			{
				float data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Type::String:
			{
				std::uint16_t length{};
				file.read(reinterpret_cast<char*>(&length), sizeof(length));
				std::wstring data(length, L'\0');
				file.read(reinterpret_cast<char*>(data.data()), length * sizeof(std::wstring::value_type));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Type::Sprite:
			{
				std::uint32_t length{};
				file.read(reinterpret_cast<char*>(&length), sizeof(length));
#if defined _CLIENT || defined _TOOL
				std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
				file.read(reinterpret_cast<char*>(binary.get()), length);

				auto data{ g_loadSprite(std::span{ binary.get(), length}) };
				prop->data.emplace<decltype(data)>(data);
#else
				file.ignore(length);
#endif
				break;
			}
			case Resource::Type::Model:
			{
				std::uint32_t length{};
				file.read(reinterpret_cast<char*>(&length), sizeof(length));
#if defined _CLIENT || defined _TOOL
				std::unique_ptr<std::byte[]> binary{ new std::byte[length]{} };
				file.read(reinterpret_cast<char*>(binary.get()), length);

				auto data{ g_loadModel(std::span{ binary.get(), length}) };
				prop->data.emplace<decltype(data)>(data);
#else
				file.ignore(length);
#endif
				break;
			}
			default:
				break;
			}

			// 자식 프로퍼티
			std::uint16_t childrenCount{};
			file.read(reinterpret_cast<char*>(&childrenCount), sizeof(childrenCount));
			for (std::uint16_t i{ 0 }; i < childrenCount; ++i)
			{
				auto child{ self() };
				prop->children.push_back(std::move(child));
			}
			return prop;
		};

		return recursive();
	}
}

namespace Resource
{
	std::shared_ptr<Resource::Property> Get(std::wstring_view path)
	{
		std::filesystem::path filePath{ path };
		std::wstring_view subPath;

		// 파일 이름, 하위 경로 분리
		constexpr std::wstring_view extension{ L".dat" };
		if (size_t pos{ path.rfind(extension) }; pos != path.npos)
		{
			filePath = path.substr(0, pos + extension.size());
			if (path.size() > pos + extension.size() + 1)
				subPath = path.substr(pos + extension.size() + 1);
		}

		// 캐시에 있는지 확인해서 반환
		if (g_resources.contains(filePath))
			return Get(g_resources[filePath], subPath);

		// 로드
		if (auto prop{ Load(filePath, subPath) })
		{
			g_resources.emplace(filePath, prop);
			if (subPath.empty())
				return prop;
			return Get(prop, subPath);
		}
		return nullptr;
	}

	std::shared_ptr<Resource::Property> Get(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
	{
		if (path.empty())
			return prop;

		size_t pos{ path.find(L'/') };
		std::wstring_view childName{ path.substr(0, pos) };
		auto it{ std::ranges::find_if(prop->children, [childName](const auto& child) { return child->name == childName; }) };
		if (it == prop->children.end())
			return nullptr;

		if (pos == path.npos)
			return *it;

		path.remove_prefix(pos + 1);
		return Get(*it, path);
	}

	std::wstring GetName(const std::shared_ptr<Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return p->name;
	}

	std::int32_t GetInt(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetInt(p);
	}

	std::int32_t GetInt(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<std::int32_t>(p->data);
	}

	Int2 GetInt2(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetInt2(p);
	}

	Int2 GetInt2(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<Int2>(p->data);
	}

	float GetFloat(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetFloat(p);
	}

	float GetFloat(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<float>(p->data);
	}

	std::wstring GetString(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetString(p);
	}

	std::wstring GetString(const std::shared_ptr<Resource::Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<std::wstring>(p->data);
	}

	std::shared_ptr<Sprite> GetSprite(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetSprite(p);
	}

	std::shared_ptr<Sprite> GetSprite(const std::shared_ptr<Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<std::shared_ptr<Sprite>>(p->data);
	}

	std::shared_ptr<Model> GetModel(std::wstring_view path)
	{
		auto p{ Get(path) };
		return GetModel(p);
	}

	std::shared_ptr<Model> GetModel(const std::shared_ptr<Property>& prop, std::wstring_view path)
	{
		auto p{ Get(prop, path) };
		return std::get<std::shared_ptr<Model>>(p->data);
	}

	void SetName(const std::shared_ptr<Property>& prop, std::wstring_view name)
	{
		prop->name = name;
	}

	void Set(const std::shared_ptr<Property>& prop, std::int32_t value)
	{
		prop->data = value;
	}

	void Set(const std::shared_ptr<Property>& prop, Int2 value)
	{
		prop->data = value;
	}

	void Set(const std::shared_ptr<Property>& prop, float value)
	{
		prop->data = value;
	}

	void Set(const std::shared_ptr<Property>& prop, std::wstring_view value)
	{
		prop->data = std::wstring{ value };
	}

	void Set(const std::shared_ptr<Property>& prop, const std::shared_ptr<Sprite>& value)
	{
		prop->data = value;
	}

	void Set(const std::shared_ptr<Property>& prop, const std::shared_ptr<Model>& value)
	{
		prop->data = value;
	}

	void Unload(std::wstring_view path)
	{
	}

#ifdef _TOOL
	std::shared_ptr<Property> New()
	{
		return std::make_shared<Property>();
	}

	std::shared_ptr<Property> Clone(const std::shared_ptr<Property>& prop)
	{
		auto clone{ std::make_shared<Property>() };
		clone->name = prop->name;
		clone->type = prop->type;
		clone->data = prop->data;
		for (const auto& child : prop->children)
		{
			auto childClone{ Clone(child) };
			clone->children.push_back(childClone);
		}
		return clone;
	}

	Type GetType(const std::shared_ptr<Property>& prop)
	{
		if (std::holds_alternative<std::int32_t>(prop->data))
			return Type::Int;
		if (std::holds_alternative<Int2>(prop->data))
			return Type::Int2;
		if (std::holds_alternative<float>(prop->data))
			return Type::Float;
		if (std::holds_alternative<std::wstring>(prop->data))
			return Type::String;
		if (std::holds_alternative<std::shared_ptr<Sprite>>(prop->data))
			return Type::Sprite;
		if (std::holds_alternative<std::shared_ptr<Model>>(prop->data))
			return Type::Model;
		return Type::Folder;
	}

	void SetType(const std::shared_ptr<Property>& prop, Type type)
	{
		switch (type)
		{
		case Resource::Type::Folder:
			break;
		case Resource::Type::Int:
			break;
		case Resource::Type::Int2:
			break;
		case Resource::Type::Float:
			break;
		case Resource::Type::String:
			break;
		case Resource::Type::Sprite:
			prop->data.emplace<std::shared_ptr<Sprite>>(nullptr);
			break;
		case Resource::Type::Model:
			break;
		default:
			break;
		}
	}

	void AddChild(const std::shared_ptr<Property>& prop, const std::shared_ptr<Property>& child)
	{
		child->parent = prop;
		prop->children.push_back(child);
	}

	std::shared_ptr<Property> GetParent(const std::shared_ptr<Property>& prop)
	{
		return prop->parent.lock();
	}

	bool Save(const std::filesystem::path& filePath, const std::shared_ptr<Resource::Property>& prop)
	{
		std::ofstream file{ filePath, std::ios::binary };
		if (!file)
			return false;

		auto recursive = [&file](this const auto& self, const std::shared_ptr<Resource::Property>& prop) -> bool
		{
			// 이름
			std::uint16_t length{ static_cast<std::uint16_t>(prop->name.size()) };
			file.write(reinterpret_cast<const char*>(&length), sizeof(length));
			file.write(reinterpret_cast<const char*>(prop->name.data()), prop->name.size() * sizeof(std::wstring::value_type));

			// 타입
			file.write(reinterpret_cast<const char*>(&prop->type), sizeof(prop->type));

			// 데이터
			switch (prop->type)
			{
			case Type::Folder:
			{
				break;
			}
			case Type::Int:
			{
				auto data{ std::get<std::int32_t>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Type::Int2:
			{
				auto data{ std::get<Int2>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Type::Float:
			{
				auto data{ std::get<float>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Type::String:
			{
				const auto& data{ std::get<std::wstring>(prop->data) };
				std::uint16_t length{ static_cast<std::uint16_t>(data.size()) };
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(std::wstring::value_type));
				break;
			}
			case Type::Sprite:
			{
				// TODO
				std::uint32_t length{};
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				break;
			}
			case Type::Model:
			{
				// TODO
				std::uint32_t length{};
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				break;
			}
			default:
				break;
			}

			// 자식
			std::uint16_t count{ static_cast<std::uint16_t>(prop->children.size()) };
			file.write(reinterpret_cast<const char*>(&count), sizeof(count));
			for (const auto& child : prop->children)
				self(child);
			return true;
		};

		std::wstring name{ prop->name };
		prop->name = L"Root";
		bool success{ recursive(prop) };
		prop->name = name;
		return success;
	}
#endif
}
