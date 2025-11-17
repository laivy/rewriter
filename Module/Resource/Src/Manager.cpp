#include "Pch.h"
#include "Delegates.h"
#include "Manager.h"

namespace
{
	// Resource::Manager::Value 순서와 동일
	enum class Type : std::uint8_t
	{
		Folder,
		Int32,
		Float,
		String,
		Sprite
	};
}

namespace Resource
{
	Manager::Manager()
	{
		Delegates::OnInitialize.Bind(this, std::bind_front(&Manager::OnInitialize, this));
		Delegates::OnFinalize.Bind(this, std::bind_front(&Manager::OnFinalize, this));
	}

	ID Manager::New(const std::wstring& path)
	{
		if (m_pathToID.contains(path))
		{
			assert(false && "already exists");
			return m_pathToID.at(path);
		}

		std::wstring name;
		ID parentID{ InvalidID };
		if (const auto pos{ path.rfind(Stringtable::DataPathSeperator) }; pos == std::wstring::npos)
		{
			name = path;
		}
		else
		{
			name = path.substr(pos + 1);
			if (!path.ends_with(Stringtable::DataFileExtension))
			{
				std::wstring parentPath{ path.substr(0, pos) };
				if (m_pathToID.contains(parentPath))
				{
					parentID = m_pathToID.at(parentPath);
				}
				else
				{
					assert(false && "parent does not exist");
					return InvalidID;
				}
			}
		}

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

		return m_pathToID.at(path);
	}

	ID Manager::New(ID parentID, const std::wstring& path)
	{
		if (!m_idToEntry.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		const auto fullPath{ std::format(L"{}{}{}", m_idToEntry.at(parentID).path, Stringtable::DataPathSeperator, path) };
		const ID id{ New(fullPath) };
		return id;
	}

	void Manager::Delete(ID id)
	{
		if (ID parentID{ GetParent(id) }; parentID != InvalidID)
			std::erase(m_idToEntry.at(parentID).children, id);

		[this](this auto self, ID id)
		{
			if (id >= m_properties.size())
			{
				assert(false && "invalid id");
				return;
			}

			auto& prop{ m_properties.at(id) };
			if (!prop)
				return;

			if (!m_idToEntry.contains(id))
			{
				assert(false && "invalid id");
				return;
			}

			auto& entry{ m_idToEntry.at(id) };
			for (ID childID : entry.children)
				self(childID);

			prop.reset();
			m_pathToID.erase(entry.path);
			m_idToEntry.erase(id);
		}(id);
	}

	ID Manager::Get(const std::wstring& path)
	{
		if (m_pathToID.contains(path))
			return m_pathToID.at(path);

		const std::size_t pos{ path.rfind(Stringtable::DataFileExtension) };
		if (pos == std::wstring::npos)
			return InvalidID;

		constexpr std::size_t ExtSize{ Stringtable::DataFileExtension.size() };
		std::filesystem::path filePath;
		std::wstring subPath;
		if (pos + ExtSize == path.size())
		{
			filePath = path;
		}
		else
		{
			filePath = path.substr(0, pos + ExtSize);
			subPath = path.substr(pos + ExtSize + 1);
		}

		// 파일이 로딩되어 있는데 위에서 찾지 못한 경우는 없는 것임
		if (m_pathToID.contains(filePath))
			return InvalidID;

		return LoadFromFile(filePath, subPath);
	}

	ID Manager::Get(ID parentID, const std::wstring& path)
	{
		if (!m_idToEntry.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		const auto fullPath{ std::format(L"{}{}{}", m_idToEntry.at(parentID).path, Stringtable::DataPathSeperator, path) };
		return Get(fullPath);
	}

	ID Manager::GetParent(ID id) const
	{
		return m_idToEntry.contains(id) ? m_idToEntry.at(id).parentID : InvalidID;
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

	std::size_t Manager::Size(ID parentID) const
	{
		if (m_idToEntry.contains(parentID))
			return m_idToEntry.at(parentID).children.size();
		return 0;
	}

	void Manager::Move(ID targetID, ID parentID, std::optional<std::size_t> index)
	{
		if (targetID >= m_properties.size() || parentID >= m_properties.size())
		{
			assert(false && "invalid id");
			return;
		}
		if (!m_properties.at(targetID) || !m_properties.at(parentID))
		{
			assert(false && "invalid id");
			return;
		}
		if (!m_idToEntry.contains(targetID) || !m_idToEntry.contains(parentID))
		{
			assert(false && "invalid id");
			return;
		}

		const std::wstring targetName{ GetName(targetID) };
		if (const ID oldParentID{ GetParent(targetID) }; oldParentID != InvalidID)
		{
			if (parentID != oldParentID)
			{
				// 이름 중복 확인
				if (std::ranges::any_of(Iterator{ parentID }, [&targetName](const auto& kv) { return targetName == kv.first; }))
					return;

				// 자신의 자식 밑으로 이동할 수 없음
				ID temp{ parentID };
				do
				{
					if (temp == targetID)
						return;
					temp = GetParent(temp);
				} while (temp != InvalidID);
			}

			// 현재 부모의 자식 목록에서 삭제
			if (m_idToEntry.contains(oldParentID))
				std::erase(m_idToEntry.at(oldParentID).children, targetID);
		}

		auto& childEntry{ m_idToEntry.at(targetID) };
		auto& parentEntry{ m_idToEntry.at(parentID) };
		childEntry.path = std::format(L"{}{}{}", parentEntry.path, Stringtable::DataPathSeperator, targetName);
		childEntry.parentID = parentID;
		if (index >= parentEntry.children.size())
			parentEntry.children.push_back(targetID);
		else
			parentEntry.children.insert(parentEntry.children.begin() + *index, targetID);
	}

	bool Manager::SetName(ID id, const std::wstring& name)
	{
		if (name.empty())
			return false;

		if (id >= m_properties.size())
		{
			assert(false && "invalid id");
			return false;
		}

		auto& prop{ m_properties.at(id) };
		if (!prop)
		{
			assert(false && "not exists");
			return false;
		}

		if (prop->name == name)
			return true;

		if (!m_idToEntry.contains(id))
		{
			assert(false && "invalid id");
			return false;
		}

		if (name.find(Stringtable::DataPathSeperator) != std::wstring::npos)
		{
			assert(false && "invalid name");
			return false;
		}

		const std::wstring oldPath{ m_idToEntry.at(id).path };
		const std::wstring newPath{ [&]()
		{
			if (const auto parentID{ GetParent(id) }; parentID != InvalidID)
				return std::format(L"{}{}{}", m_idToEntry.at(parentID).path, Stringtable::DataPathSeperator, name);
			return name;
		}() };

		if (m_pathToID.contains(newPath))
			return false;

		prop->name = name;
		m_idToEntry.at(id).path = newPath;
		m_pathToID.erase(oldPath);
		m_pathToID.emplace(newPath, id);
		return true;
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

	std::wstring Manager::GetPath(ID id) const
	{
		if (!m_idToEntry.contains(id))
		{
			assert(false && "invalid id");
			return L"";
		}
		return m_idToEntry.at(id).path;
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

		const auto tempFilePath{ std::filesystem::temp_directory_path() / path.filename() };
		std::ofstream tempFile{ tempFilePath, std::ios::binary };
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
					auto dataLength{ static_cast<std::uint32_t>(arg.binary->size()) };
					tempFile.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
					tempFile.write(reinterpret_cast<const char*>(arg.binary->data()), dataLength * sizeof(std::byte));
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
		tempFile.close();

		if (std::filesystem::exists(path) && !std::filesystem::remove(path))
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
	}

	void Manager::OnFinalize()
	{
		m_mountPath.clear();
		m_loadSprite = nullptr;
		m_properties.clear();
		m_idToEntry.clear();
		m_pathToID.clear();
	}

	std::wstring Manager::NormalizePath(const std::wstring& path) const
	{
		constexpr auto separator{ Stringtable::DataPathSeperator.front() };
		if (std::filesystem::path::preferred_separator == separator)
			return path;

		std::wstring result{ path };
		std::ranges::replace(result, std::filesystem::path::preferred_separator, separator);
		return result;
	}

	ID Manager::LoadFromFile(const std::filesystem::path& filePath, const std::wstring& subPath)
	{
		std::ifstream file{ filePath, std::ios::binary };
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
			Type valueType{};
			file.read(reinterpret_cast<char*>(&valueType), sizeof(valueType));

			// 값
			switch (valueType)
			{
			case Type::Folder:
			{
				break;
			}
			case Type::Int32:
			{
				std::int32_t value{};
				file.read(reinterpret_cast<char*>(&value), sizeof(value));
				Set(id, value);
				break;
			}
			case Type::Float:
			{
				float value{};
				file.read(reinterpret_cast<char*>(&value), sizeof(value));
				Set(id, value);
				break;
			}
			case Type::String:
			{
				std::uint16_t dataLength{};
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::wstring value(dataLength, L'\0');
				file.read(reinterpret_cast<char*>(value.data()), dataLength * sizeof(std::wstring::value_type));
				Set(id, value);
				break;
			}
			case Type::Sprite:
			{
				std::uint32_t dataLength{};
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::vector<char> binary(dataLength);
				file.read(reinterpret_cast<char*>(binary.data()), dataLength * sizeof(char));
				const auto sprite{ m_loadSprite(id, binary) };
				Set(id, sprite);
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

		const auto normalizedFilePath{ NormalizePath(filePath) };
		const ID rootID{ New(normalizedFilePath) };

		std::uint32_t childCount{};
		file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
		for (std::uint32_t i{ 0 }; i < childCount; ++i)
		{
			const ID id{ recurse(rootID) };
			if (id == InvalidID)
				continue;
		}

		if (subPath.empty())
			return m_pathToID.at(normalizedFilePath);

		const auto path{ std::format(L"{}{}{}", normalizedFilePath, Stringtable::DataPathSeperator, subPath) };
		return m_pathToID.at(path);
	}
}
