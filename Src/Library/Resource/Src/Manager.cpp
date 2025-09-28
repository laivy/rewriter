#include "Stdafx.h"
#include "Delegates.h"
#include "Manager.h"

namespace Resource
{
	Manager::Manager()
	{
		Delegates::OnInitialize.Register(this, std::bind_front(&Manager::OnInitialize, this));
		Delegates::OnUninitialize.Register(this, std::bind_front(&Manager::OnUninitialize, this));
	}

	ID Manager::New(const std::wstring& path)
	{
		if (m_pathToID.contains(path))
		{
			assert(false && "already exists");
			return m_pathToID.at(path);
		}

		auto create = [this](const std::wstring& path, ID parentID)
		{
			if (m_pathToID.contains(path))
				return;

			std::wstring name;
			if (const auto pos{ path.rfind(L'/') }; pos == std::wstring::npos)
				name = path;
			else
				name = path.substr(pos + 1);

			auto it{ std::ranges::find_if(m_properties, [](const auto& prop) { return !prop.has_value(); }) };
			if (it == m_properties.end())
				it = m_properties.emplace(m_properties.end());
			it->emplace(name, std::monostate{});

			const ID id{ static_cast<ID>(std::distance(m_properties.begin(), it)) };
			m_pathToID.emplace(path, id);
			m_idToEntry.emplace(id, Entry{ .path = path, .parentID = InvalidID });

			// 부모, 자식 관계 설정
			if (parentID != InvalidID)
			{
				m_idToEntry.at(id).parentID = parentID;
				if (!std::ranges::contains(m_idToEntry.at(parentID).children, id))
					m_idToEntry.at(parentID).children.push_back(id);
			}
		};

		// 상위 경로 프로퍼티가 없으면 생성
		for (std::wstring subPath; const auto part : std::views::split(path, L'/'))
		{
			ID parentID{ InvalidID };
			if (subPath.empty())
			{
				subPath = std::wstring_view{ part };
			}
			else
			{
				if (m_pathToID.contains(subPath))
					parentID = m_pathToID.at(subPath);
				subPath = std::format(L"{}/{}", subPath, std::wstring_view{ part });
			}
			create(subPath, parentID);
		}

		return m_pathToID.at(path);
	}

	ID Manager::New(ID parentID, const std::wstring& path)
	{
		if (!m_idToEntry.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		const auto fullPath{ std::format(L"{}/{}", m_idToEntry.at(parentID).path, path) };
		const ID id{ New(fullPath) };
		return id;
	}

	void Manager::Delete(ID id)
	{
		if (id >= m_properties.size())
		{
			assert(false && "invalid id");
			return;
		}
		auto& prop{ m_properties.at(id) };
		if (!prop)
		{
			assert(false && "not exists");
			return;
		}
		if (!m_idToEntry.contains(id))
		{
			assert(false && "invalid id");
			return;
		}

		auto& entry{ m_idToEntry.at(id) };
		if (const ID parentID{ entry.parentID }; parentID != InvalidID)
			std::erase(m_idToEntry.at(parentID).children, id);

		for (ID childID : entry.children)
		{
			m_properties.at(childID).reset();
			m_pathToID.erase(m_idToEntry.at(childID).path);
			m_idToEntry.erase(childID);
		}

		prop.reset();
		m_pathToID.erase(entry.path);
		m_idToEntry.erase(id);
	}

	ID Manager::Get(const std::wstring& path) const
	{
		if (m_pathToID.contains(path))
			return m_pathToID.at(path);
		return InvalidID;
	}

	ID Manager::Get(ID parentID, const std::wstring& path) const
	{
		if (!m_idToEntry.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		const auto fullPath{ std::format(L"{}/{}", m_idToEntry.at(parentID).path, path) };
		return Get(fullPath);
	}

	ID Manager::GetParent(ID id) const
	{
		if (!m_idToEntry.contains(id))
		{
			assert(false && "invalid id");
			return InvalidID;
		}
		return m_idToEntry.at(id).parentID;
	}

	ID Manager::GetChild(ID parentID, std::size_t index) const
	{
		if (!m_idToEntry.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		if (index >= m_idToEntry.at(parentID).children.size())
		{
			assert(false && "invalid child index");
			return InvalidID;
		}
		return m_idToEntry.at(parentID).children.at(index);
	}

	std::size_t Manager::GetChildCount(ID parentID) const
	{
		if (!m_idToEntry.contains(parentID))
			return 0;
		return m_idToEntry.at(parentID).children.size();
	}

	void Manager::SetName(ID id, const std::wstring& name)
	{
		if (id >= m_properties.size())
		{
			assert(false && "invalid id");
			return;
		}
		auto& prop{ m_properties.at(id) };
		if (!prop)
		{
			assert(false && "not exists");
			return;
		}
		if (!m_idToEntry.contains(id))
		{
			assert(false && "invalid id");
			return;
		}
		if (name.find(L'/') != std::wstring::npos)
		{
			assert(false && "invalid name");
			return;
		}

		const std::wstring oldPath{ m_idToEntry.at(id).path };
		const std::wstring newPath{ [&]()
		{
			if (const auto parentID{ GetParent(id) }; parentID != InvalidID)
				return std::format(L"{}/{}", m_idToEntry.at(parentID).path, name);
			return name;
		}() };

		if (m_pathToID.contains(newPath))
		{
			assert(false && "already exists");
			return;
		}
		prop->name = name;
		m_idToEntry.at(id).path = newPath;
		m_pathToID.erase(oldPath);
		m_pathToID.emplace(newPath, id);
	}

	std::wstring Manager::GetName(ID id) const
	{
		if (id >= m_properties.size())
		{
			assert(false && "invalid id");
			return L"";
		}
		const auto& prop{ m_properties.at(id) };
		if (!prop)
		{
			assert(false && "not exists");
			return L"";
		}
		return prop->name;
	}

	bool Manager::SaveToFile(ID id, const std::filesystem::path& path) const
	{
		if (id >= m_properties.size())
		{
			assert(false && "invalid id");
			return false;
		}
		if (!m_properties.at(id))
		{
			assert(false && "invalid id");
			return false;
		}
		if (!m_idToEntry.contains(id))
		{
			assert(false && "there is nothing to save");
			return false;
		}
		if (m_idToEntry.at(id).children.empty())
		{
			assert(false && "there is nothing to save");
			return false;
		}

		const auto tempFilePath{ std::filesystem::temp_directory_path() / path.filename() };
		std::ofstream tempFile{ tempFilePath , std::ios::binary };
		if (!tempFile)
		{
			assert(false && "failed to open file");
			return false;
		}

		// 시그니처
		tempFile.write(Signature.data(), Signature.size());

		// 버전
		tempFile.write(reinterpret_cast<const char*>(&Version), sizeof(Version));

		auto recurse = [this, &tempFile](this auto self, ID id, const Property& prop)
		{
			// 이름
			const auto nameSize{ static_cast<std::uint16_t>(prop.name.size()) };
			tempFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
			tempFile.write(reinterpret_cast<const char*>(prop.name.data()), nameSize * sizeof(std::wstring::value_type));

			// 값 타입
			const auto valueType{ static_cast<std::uint8_t>(prop.value.index()) };
			tempFile.write(reinterpret_cast<const char*>(&valueType), sizeof(valueType));

			// 값
			std::visit([&tempFile](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::int32_t>)
				{
					tempFile.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					tempFile.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
				}
				else if constexpr (std::is_same_v<T, std::wstring>)
				{
					auto dataLength{ static_cast<std::uint16_t>(arg.size()) };
					tempFile.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
					tempFile.write(reinterpret_cast<const char*>(arg.data()), dataLength * sizeof(std::wstring::value_type));
				}
				else if constexpr (std::is_same_v<T, Resource::Sprite>)
				{
					auto dataLength{ static_cast<std::uint32_t>(arg.binary.size()) };
					tempFile.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
					tempFile.write(reinterpret_cast<const char*>(arg.binary.data()), dataLength * sizeof(std::byte));
				}
			}, prop.value);

			// 자식
			if (!m_idToEntry.contains(id))
			{
				constexpr std::uint32_t childCount{ 0 };
				tempFile.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
				return;
			}

			const auto& children{ m_idToEntry.at(id).children };
			const auto childCount{ static_cast<std::uint32_t>(children.size()) };
			tempFile.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
			for (const auto& childID : children)
			{
				if (const auto& child{ m_properties.at(childID) })
					self(childID, *child);
			}
		};

		const auto& children{ m_idToEntry.at(id).children };
		const auto childCount{ static_cast<std::uint32_t>(children.size()) };
		tempFile.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
		for (ID childID : children)
		{
			if (const auto& child{ m_properties.at(childID) })
				recurse(childID, *child);
		}

		if (!std::filesystem::remove(path))
		{
			assert(false && "failed to remove existing file");
			return false;
		}
		std::filesystem::rename(tempFilePath, path);
		return true;
	}

	void Manager::OnInitialize(const Initializer& initializer)
	{
		m_mountPath = initializer.mountPath;
		m_loadSprite = initializer.loadSprite;
		m_loadModel = initializer.loadModel;
	}

	void Manager::OnUninitialize()
	{
		m_mountPath.clear();
		m_loadSprite = nullptr;
		m_loadModel = nullptr;
		m_properties.clear();
		m_idToEntry.clear();
		m_pathToID.clear();
	}

	ID Manager::LoadFromFile(const std::filesystem::path& path, const std::wstring& subPath)
	{
		std::ifstream file{ path, std::ios::binary };
		if (!file)
		{
			assert(false && "failed to open file");
			return InvalidID;
		}

		// 시그니처
		std::array<char, Signature.size()> signature{};
		file.read(signature.data(), signature.size());
		if (!std::ranges::equal(signature, Signature))
		{
			assert(false && "invalid file format");
			return InvalidID;
		}

		// 버전
		struct Version
		{
			std::uint8_t major;
			std::uint8_t minor;
			std::uint8_t patch;
		};
		Version version{};
		file.read(reinterpret_cast<char*>(&version), sizeof(version));

		auto recurse = [this, &file](this auto self, ID parentID)
		{
			// 이름
			std::uint16_t nameSize{};
			file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
			std::wstring name(nameSize, L'\0');
			file.read(reinterpret_cast<char*>(name.data()), nameSize * sizeof(std::wstring::value_type));

			const ID id{ parentID == InvalidID ? New(name) : New(parentID, name) };
			if (id == InvalidID)
			{
				assert(false && "failed to create new property");
				return InvalidID;
			}

			// 값 타입
			std::uint8_t valueType{};
			file.read(reinterpret_cast<char*>(&valueType), sizeof(valueType));

			// 값
			switch (valueType)
			{
			case 0: // std::monostate
			{
				break;
			}
			case 1: // std::int32_t
			{
				std::int32_t value{};
				file.read(reinterpret_cast<char*>(&value), sizeof(value));
				Set(id, value);
				break;
			}
			case 2: // float
			{
				float value{};
				file.read(reinterpret_cast<char*>(&value), sizeof(value));
				Set(id, value);
				break;
			}
			case 3: // std::wstring
			{
				std::uint16_t dataLength{};
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::wstring value(dataLength, L'\0');
				file.read(reinterpret_cast<char*>(value.data()), dataLength * sizeof(std::wstring::value_type));
				Set(id, value);
				break;
			}
			case 4: // Resource::Sprite
			{
				std::uint32_t dataLength{};
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::vector<std::byte> binary(dataLength);
				file.read(reinterpret_cast<char*>(binary.data()), dataLength * sizeof(std::byte));
				const auto sprite{ m_loadSprite(binary) };
				//Set(id, sprite);
				break;
			}
			default:
				assert(false && "invalid value type");
				return InvalidID;
			}

			// 자식
			std::uint32_t childCount{};
			file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
			for (std::uint32_t i{ 0 }; i < childCount; ++i)
			{
				const ID child{ self(id) };
				if (child == InvalidID)
				{
					assert(false && "failed to load child");
					return InvalidID;
				}
			}
			return id;
		};

		std::uint32_t childCount{};
		file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
		for (std::uint32_t i{ 0 }; i < childCount; ++i)
		{
			const ID id{ recurse(InvalidID) };
			if (id == InvalidID)
				continue;
		}

		return m_pathToID.at(path / subPath);
	}
}
