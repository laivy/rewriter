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

	ID Manager::New(const std::wstring& name)
	{
		if (m_pathToID.contains(name))
		{
			assert(false && "already exists");
			return m_pathToID.at(name);
		}
		if (name.find(L'/') != std::wstring::npos)
		{
			assert(false && "invalid name");
			return InvalidID;
		}

		auto it{ std::ranges::find_if(m_properties, [](const auto& prop) { return !prop.has_value(); }) };
		if (it == m_properties.end())
			it = m_properties.emplace(m_properties.end());
		it->emplace(name, std::monostate{});

		const ID id{ static_cast<ID>(std::distance(m_properties.begin(), it)) };
		m_idToPath.emplace(id, name);
		m_pathToID.emplace(name, id);
		return id;
	}

	ID Manager::New(ID parentID, const std::wstring& name)
	{
		if (!m_idToPath.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		if (name.find(L'/') != std::wstring::npos)
		{
			assert(false && "invalid name");
			return InvalidID;
		}

		auto it{ std::ranges::find_if(m_properties, [](const auto& prop) { return !prop.has_value(); }) };
		if (it == m_properties.end())
			it = m_properties.emplace(m_properties.end());
		it->emplace(name, std::monostate{});

		const ID id{ static_cast<ID>(std::distance(m_properties.begin(), it)) };
		m_entries[id].parentID = parentID;
		m_entries[parentID].children.push_back(id);

		const auto fullPath{ std::format(L"{}/{}", m_idToPath.at(parentID), name) };
		m_idToPath.emplace(id, fullPath);
		m_pathToID.emplace(fullPath, id);
		return id;
	}

	ID Manager::Get(const std::wstring& path) const
	{
		if (!m_pathToID.contains(path))
		{
			assert(false && "invalid path");
			return InvalidID;
		}
		return m_pathToID.at(path);
	}

	ID Manager::Get(ID parentID, const std::wstring& path) const
	{
		if (!m_idToPath.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		const auto fullPath{ std::format(L"{}/{}", m_idToPath.at(parentID), path) };
		return Get(fullPath);
	}

	ID Manager::GetParent(ID id) const
	{
		if (!m_entries.contains(id))
		{
			assert(false && "invalid id");
			return InvalidID;
		}
		return m_entries.at(id).parentID;
	}

	ID Manager::GetChild(ID parentID, std::size_t index) const
	{
		if (!m_entries.contains(parentID))
		{
			assert(false && "invalid parent id");
			return InvalidID;
		}
		if (index >= m_entries.at(parentID).children.size())
		{
			assert(false && "invalid child index");
			return InvalidID;
		}
		return m_entries.at(parentID).children.at(index);
	}

	std::size_t Manager::GetChildCount(ID parentID) const
	{
		if (!m_entries.contains(parentID))
			return 0;
		return m_entries.at(parentID).children.size();
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
		if (!m_idToPath.contains(id))
		{
			assert(false && "invalid id");
			return;
		}
		if (name.find(L'/') != std::wstring::npos)
		{
			assert(false && "invalid name");
			return;
		}

		const std::wstring oldPath{ m_idToPath.at(id) };
		const std::wstring newPath{ [&]()
		{
			if (const auto parentID{ GetParent(id) }; parentID != InvalidID)
				return std::format(L"{}/{}", m_idToPath.at(parentID), name);
			return name;
		}() };

		if (m_pathToID.contains(newPath))
		{
			assert(false && "already exists");
			return;
		}
		prop->name = name;
		m_idToPath.at(id) = newPath;
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
		if (!m_entries.contains(id))
		{
			assert(false && "there is nothing to save");
			return false;
		}
		if (m_entries.at(id).children.empty())
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
			if (!m_entries.contains(id))
			{
				constexpr std::uint32_t childCount{ 0 };
				tempFile.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
				return;
			}

			const auto& children{ m_entries.at(id).children };
			const auto childCount{ static_cast<std::uint32_t>(children.size()) };
			tempFile.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
			for (const auto& childID : children)
			{
				if (const auto& child{ m_properties.at(childID) })
					self(childID, *child);
			}
		};

		const auto& children{ m_entries.at(id).children };
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
		m_idToPath.clear();
		m_pathToID.clear();
	}
}
