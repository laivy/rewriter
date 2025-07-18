module rewriter.library.resource:property;

import std;
import :model;
import :global;
import :sprite;
import rewriter.common.type;

namespace Resource
{
	struct Property
	{
		enum class Type : std::uint8_t
		{
			Folder,
			Int,
			Int2,
			Float,
			String,
			Sprite,
			Model,
		};

		std::wstring name;
		Type type;
		std::variant<
			std::int32_t,
			Int2,
			float,
			std::wstring,
			std::shared_ptr<Sprite>,
			std::shared_ptr<Model>
		> data;
		std::vector<std::shared_ptr<Resource::Property>> children;
	};
}

namespace
{
	std::shared_ptr<Resource::Property> Load(const std::filesystem::path& filePath, std::wstring_view subPath)
	{
		std::ifstream file;
		if (filePath.is_absolute())
			file.open(filePath, std::ios::binary);
		else
			file.open(L"Data" / filePath, std::ios::binary);

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
			case Resource::Property::Type::Folder:
			{
				break;
			}
			case Resource::Property::Type::Int:
			{
				std::int32_t data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Property::Type::Int2:
			{
				Int2 data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Property::Type::Float:
			{
				float data{};
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Property::Type::String:
			{
				std::uint16_t length{};
				file.read(reinterpret_cast<char*>(&length), sizeof(length));
				std::wstring data(length, L'\0');
				file.read(reinterpret_cast<char*>(data.data()), length * sizeof(std::wstring::value_type));
				prop->data.emplace<decltype(data)>(data);
				break;
			}
			case Resource::Property::Type::Sprite:
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
			case Resource::Property::Type::Model:
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

	void Unload(std::wstring_view path)
	{
	}

#ifdef _TOOL
	bool Save(const std::filesystem::path& filePath, const std::shared_ptr<Resource::Property>& prop)
	{
		std::ofstream file{ filePath, std::ios::binary };
		if (!file)
			return false;

		auto recursive = [&file](this const auto& self, const std::shared_ptr<Resource::Property>& prop) -> bool
		{
			// 이름
			uint16_t length{ static_cast<uint16_t>(prop->name.size()) };
			file.write(reinterpret_cast<const char*>(&length), sizeof(length));
			file.write(reinterpret_cast<const char*>(prop->name.data()), prop->name.size() * sizeof(std::wstring::value_type));

			// 타입
			file.write(reinterpret_cast<const char*>(&prop->type), sizeof(prop->type));

			// 데이터
			switch (prop->type)
			{
			case Property::Type::Folder:
			{
				break;
			}
			case Property::Type::Int:
			{
				auto data{ std::get<std::int32_t>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Property::Type::Int2:
			{
				auto data{ std::get<Int2>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Property::Type::Float:
			{
				auto data{ std::get<float>(prop->data) };
				file.write(reinterpret_cast<const char*>(&data), sizeof(data));
				break;
			}
			case Property::Type::String:
			{
				const auto& data{ std::get<std::wstring>(prop->data) };
				std::uint16_t length{ static_cast<std::uint16_t>(data.size()) };
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(std::wstring::value_type));
				break;
			}
			case Property::Type::Sprite:
			{
				// TODO
				std::uint32_t length{};
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				break;
			}
			case Property::Type::Model:
			{
				// TODO
				std::uint32_t length{};
				file.write(reinterpret_cast<const char*>(&length), sizeof(length));
				break;
			}
			default:
				break;
			}

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
