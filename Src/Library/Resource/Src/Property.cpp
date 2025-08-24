#include "Stdafx.h"
#include "Delegates.h"
#include "Property.h"

namespace
{
	using Value = std::variant<
		std::monostate,
		std::int32_t,
		float,
		std::wstring,
		Resource::Sprite
	>;

	struct Entry
	{
		Resource::Property::ID id;
		std::wstring name;
		Value value;
	};

	constexpr std::array Signature{ 'l', 'v', 'y' };
	constexpr std::uint32_t Version{ 1U };

	std::function<Resource::Sprite(std::span<std::byte>)> g_loadSprite;

	std::vector<Entry> g_entries;
	std::unordered_map<Resource::Property::ID, std::wstring> g_hashes;
	std::unordered_map<Resource::Property::ID, std::vector<Resource::Property::ID>> g_children;

	auto _ = []()
	{
		OnInitialize.Register([](const Resource::Initializer& initializer)
		{
			g_loadSprite = initializer.loadSprite;
		});

		OnUninitialize.Register([]()
		{
			g_entries.clear();
			g_hashes.clear();
			g_children.clear();
		});
		return true;
	}();

	Resource::Property::ID Hash(std::wstring_view name)
	{
		return std::hash<std::wstring_view>{}(name);
	}

	void Register(const Entry& entry, std::wstring_view fullPath, const std::optional<Resource::Property::ID> parentID = std::nullopt)
	{
		auto it{ std::ranges::lower_bound(g_entries, entry.id, std::less{}, [](const auto& data) { return data.id; }) };
		g_entries.insert(it, entry);
		g_hashes.emplace(entry.id, fullPath);
		if (parentID && *parentID != Resource::Property::InvalidID)
			g_children[*parentID].push_back(entry.id);
	}

	std::optional<std::reference_wrapper<Entry>> GetEntry(const Resource::Property::ID id)
	{
		auto entry{ std::ranges::lower_bound(g_entries, id, std::less{}, [](const auto& data) { return data.id; }) };
		if (entry == g_entries.end())
			return std::nullopt;
		if (entry->id != id)
			return std::nullopt;
		return std::ref(*entry);
	}

	bool Save(const Resource::Property::ID id, const std::filesystem::path& path)
	{
		if (!g_children.contains(id))
			return false;

		auto entry{ GetEntry(id) };
		if (!entry)
			return false;

		std::ofstream file{ path, std::ios::binary };
		if (!file)
			return false;

		// 시그니처
		file.write(Signature.data(), Signature.size());

		// 버전
		file.write(reinterpret_cast<const char*>(&Version), sizeof(Version));

		auto recursive = [&file](this auto self, const Entry& entry)
		{
			// 이름
			const auto nameSize{ static_cast<std::uint16_t>(entry.name.size()) };
			file.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
			file.write(reinterpret_cast<const char*>(entry.name.data()), nameSize * sizeof(std::wstring::value_type));

			// 값 타입
			const auto valueType{ static_cast<std::uint8_t>(entry.value.index()) };
			file.write(reinterpret_cast<const char*>(&valueType), sizeof(valueType));

			// 값
			std::visit([&file](auto&& arg)
			{
				using T = std::decay_t< decltype(arg) >;
				if constexpr (std::is_same_v<T, std::int32_t>)
				{
					file.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					file.write(reinterpret_cast<const char*>(&arg), sizeof(arg));
				}
				else if constexpr (std::is_same_v<T, std::wstring>)
				{
					auto dataLength{ static_cast<std::uint16_t>(arg.size()) };
					file.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
					file.write(reinterpret_cast<const char*>(arg.data()), dataLength * sizeof(std::wstring::value_type));
				}
				else if constexpr (std::is_same_v<T, Resource::Sprite>)
				{
					auto dataLength{ static_cast<std::uint32_t>(arg.binary.size()) };
					file.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
					file.write(reinterpret_cast<const char*>(arg.binary.data()), dataLength * sizeof(std::byte));
				}
			}, entry.value);

			// 자식
			if (!g_children.contains(entry.id))
			{
				constexpr std::uint32_t childCount{ 0 };
				file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
				return;
			}

			const auto& children{ g_children[entry.id] };
			const auto childCount{ static_cast<std::uint32_t>(children.size()) };
			file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
			for (const auto& childID : children)
			{
				auto child{ GetEntry(childID) };
				if (!child)
					continue;
				self(*child);
			}
		};

		// 저장
		const auto& children{ g_children[id] };
		const auto childCount{ static_cast<std::uint32_t>(children.size()) };
		file.write(reinterpret_cast<const char*>(&childCount), sizeof(childCount));
		for (const auto& childID : children)
		{
			auto child{ GetEntry(childID) };
			if (!child)
				continue;
			recursive(*child);
		}
		return true;
	}

	bool Load(std::wstring_view fullPath)
	{
		// 파일 경로와 세부 경로 분리
		constexpr std::wstring_view Extension{ Stringtable::DATA_FILE_EXT };
		std::filesystem::path filePath;
		std::wstring_view subPath;
		if (fullPath.ends_with(Extension))
		{
			filePath = fullPath;
		}
		else
		{
			std::size_t pos{ fullPath.rfind(Extension) };
			filePath = fullPath.substr(0, pos + Extension.size());
			subPath = fullPath.substr(pos + Extension.size() + 1);
		}

		if (filePath.extension() != Extension)
			return false;

		std::ifstream file{ filePath, std::ios::binary };
		if (!file)
			return false;

		// 시그니처 확인
		std::array<char, Signature.size()> signature{};
		file.read(signature.data(), signature.size());
		if (!std::ranges::equal(signature, Signature))
			return false;

		// 버전 확인
		std::uint32_t version{ 0 };
		file.read(reinterpret_cast<char*>(&version), sizeof(version));

		auto recursive = [&file, version](this auto self, std::wstring_view parentPath = L"") -> Resource::Property::ID
		{
			// 이름
			std::uint16_t nameSize{ 0 };
			file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
			std::wstring name(nameSize, L'\0');
			file.read(reinterpret_cast<char*>(name.data()), nameSize * sizeof(std::wstring::value_type));

			// 값 타입
			std::uint8_t valueType{ 0 };
			file.read(reinterpret_cast<char*>(&valueType), sizeof(valueType));

			// 값
			Value value;
			switch (valueType)
			{
			case 0: // 폴더
			{
				break;
			}
			case 1: // 정수
			{
				std::int32_t data{ 0 };
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				value = data;
				break;
			}
			case 2: // 실수
			{
				float data{ 0.0f };
				file.read(reinterpret_cast<char*>(&data), sizeof(data));
				value = data;
				break;
			}
			case 3: // 문자열
			{
				std::uint16_t dataLength{ 0 };
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::wstring data(dataLength, L'\0');
				file.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(std::wstring::value_type));
				value = std::move(data);
				break;
			}
			case 4: // .png
			{
				std::uint32_t dataLength{ 0 };
				file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
				std::vector<std::byte> data(dataLength);
				file.read(reinterpret_cast<char*>(data.data()), dataLength * sizeof(std::byte));
				value = g_loadSprite(data);
				break;
			}
			default:
				assert(false && "Unknown value type");
				return Resource::Property::InvalidID;
			}

			// 등록
			const auto fullPath{ std::format(L"{}/{}", parentPath, name) };
			const auto id{ Hash(fullPath) };
			g_entries.emplace_back(id, std::move(name), std::move(value));
			g_hashes.emplace(id, fullPath);

			// 자식
			std::uint32_t childCount{ 0 };
			file.read(reinterpret_cast<char*>(&childCount), sizeof(childCount));
			for (std::uint32_t i{ 0 }; i < childCount; ++i)
			{
				const auto childID{ self(fullPath) };
				g_children[id].push_back(childID);
			}
			return id;
		};

		const auto RootID{ Hash(filePath.wstring()) };
		if (!g_hashes.contains(RootID))
			g_hashes.emplace(RootID, filePath.wstring());

		std::uint32_t count{ 0 };
		file.read(reinterpret_cast<char*>(&count), sizeof(count));
		for (std::uint32_t i{ 0 }; i < count; ++i)
			recursive(fullPath);
		return true;
	}
}

namespace Resource::Property
{
	Iterator::Iterator() :
		m_parentID{ InvalidID },
		m_current{ 0 },
		m_end{ 0 }
	{
	}

	Iterator::Iterator(const ID id) :
		m_parentID{ id },
		m_current{ 0 },
		m_end{ 0 }
	{
		if (g_children.contains(m_parentID))
			m_end = g_children[m_parentID].size();
	}

	Iterator::Iterator(std::wstring_view path) :
		Iterator{ Get(path) }
	{
	}

	Iterator::value_type Iterator::operator*() const
	{
		const auto id{ g_children[m_parentID][m_current] };
		auto entry{ GetEntry(id) };
		if (!entry)
			return { L"", InvalidID };
		return { entry->get().name, id };
	}

	Iterator& Iterator::operator++()
	{
		++m_current;
		return *this;
	}

	Iterator Iterator::operator++(int)
	{
		auto it{ *this };
		++m_current;
		return it;
	}

	Iterator& Iterator::operator--()
	{
		--m_current;
		return *this;
	}

	Iterator Iterator::operator--(int)
	{
		auto it{ *this };
		--m_current;
		return it;
	}

	Iterator& Iterator::operator+=(const difference_type offset)
	{
		m_current += offset;
		return *this;
	}

	Iterator Iterator::operator+(const difference_type offset) const
	{
		auto it{ *this };
		it.m_current += offset;
		return it;
	}

	Iterator& Iterator::operator-=(const difference_type offset)
	{
		m_current -= offset;
		return *this;
	}

	Iterator Iterator::operator-(const difference_type offset) const
	{
		auto it{ *this };
		it.m_current -= offset;
		return it;
	}

	Iterator::difference_type Iterator::operator-(const Iterator& other) const
	{
		if (m_parentID != other.m_parentID)
		{
			assert(false && "different parentID");
			return 0;
		}
		return static_cast<difference_type>(m_current) - static_cast<difference_type>(other.m_current);
	}

	Iterator::value_type Iterator::operator[](const difference_type offset) const
	{
		const auto index{ static_cast<std::size_t>(static_cast<difference_type>(m_current) + offset) };
		if (index >= m_end)
		{
			assert(false && "out of range");
			return { L"", InvalidID };
		}

		const auto id{ g_children[m_parentID][index] };
		auto entry{ GetEntry(id) };
		if (!entry)
			return { L"", InvalidID };
		return { entry->get().name, id };
	}

	bool Iterator::operator==(const Iterator& other) const
	{
		if (m_parentID != other.m_parentID)
			return false;
		if (m_current != other.m_current)
			return false;
		if (m_end != other.m_end)
			return false;
		return true;
	}

	Iterator Iterator::begin() const
	{
		return *this;
	}

	Iterator Iterator::end() const
	{
		auto it{ *this };
		it.m_current = it.m_end;
		return it;
	}

	ID New(std::wstring_view name)
	{
		if (name.find(L'/') != std::wstring_view::npos)
		{
			assert(false && "name should not contain '/'");
			return InvalidID;
		}

		const ID id{ Hash(name) };
		if (g_hashes.contains(id))
		{
			assert(false && "already exists");
			return id;
		}

		Entry entry{};
		entry.id = id;
		entry.name = name;
		Register(entry, name);
		return id;
	}

	ID Property::New(const ID parentID, std::wstring_view name)
	{
		if (name.find(L'/') != std::wstring_view::npos)
		{
			assert(false && "name should not contain '/'");
			return InvalidID;
		}

		auto parent{ GetEntry(parentID) };
		if (!parent)
		{
			assert(false && "parent not found");
			return InvalidID;
		}

		if (!g_hashes.contains(parentID))
		{
			assert(false && "parent not found");
			return InvalidID;
		}

		auto fullPath{ std::format(L"{}/{}", g_hashes[parentID], name) };
		const ID id{ Hash(fullPath) };

		Entry entry{};
		entry.id = id;
		entry.name = name;
		Register(entry, fullPath, parentID);
		return id;
	}

	ID Get(std::wstring_view fullPath)
	{
		const ID id{ Hash(fullPath) };
		if (g_hashes.contains(id))
			return id;
		if (!Load(fullPath))
			return InvalidID;
		return id;
	}

	ID Get(const ID id, std::wstring_view fullPath)
	{
		if (!g_hashes.contains(id))
			return InvalidID;

		auto key{ std::format(L"{}/{}", g_hashes[id], fullPath) };
		const ID targetID{ Hash(key) };

		auto base{ std::lower_bound(g_entries.begin(), g_entries.end(), targetID,
			[](const Entry& data, ID id)
			{
				return data.id < id;
			}) };
		if (base == g_entries.end())
			return InvalidID;
		if (base->id != targetID)
			return InvalidID;
		return targetID;
	}

	ID Property::GetParent(const ID id)
	{
		if (!g_hashes.contains(id))
			return InvalidID;

		const auto fullPath{ g_hashes[id] };
		const auto pos{ fullPath.rfind(L'/') };
		if (pos == std::wstring_view::npos)
			return InvalidID;

		auto parentPath{ fullPath.substr(0, pos) };
		const ID parentID{ Hash(parentPath) };
		if (!g_hashes.contains(parentID))
			return InvalidID;
		return parentID;
	}

	std::wstring GetPath(const ID id)
	{
		if (g_hashes.contains(id))
			return g_hashes[id];
		return L"";
	}

	void SetName(const ID id, std::wstring_view name)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
		{
			assert(false && "not found");
			return;
		}
		const auto newID{ Hash(name) };
		if (g_hashes.contains(newID))
		{
			assert(false && "already exists");
			return;
		}

		entry->get().name = name;

		if (const auto parentID{ GetParent(id) }; parentID != InvalidID)
		{
			auto it{ std::ranges::find(g_children[parentID], id) };
			if (it != g_children[parentID].end())
				*it = newID;
		}

		auto hashHandle{ g_hashes.extract(id) };
		hashHandle.key() = newID;
		g_hashes.insert(std::move(hashHandle));

		auto childrenHandle{ g_children.extract(id) };
		childrenHandle.key() = newID;
		g_children.insert(std::move(childrenHandle));
	}

	void Set(const ID id, std::int32_t value)
	{
		if (auto entry{ GetEntry(id) })
			entry->get().value = value;
	}

	void Set(const ID id, float value)
	{
		if (auto entry{ GetEntry(id) })
			entry->get().value = value;
	}

	void Set(const ID id, std::wstring_view value)
	{
		if (auto entry{ GetEntry(id) })
			entry->get().value.emplace<std::wstring>(value);
	}

	void Set(const ID id, const Sprite& value)
	{
		if (auto entry{ GetEntry(id) })
			entry->get().value = value;
	}

	std::optional<std::wstring> Property::GetName(const ID id)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
			return std::nullopt;
		return entry->get().name;
	}

	std::optional<std::int32_t> GetInt(const ID id)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
			return std::nullopt;

		const auto& value{ entry->get().value };
		if (!std::holds_alternative<std::int32_t>(value))
			return std::nullopt;
		return std::get<std::int32_t>(value);
	}

	std::optional<float> Property::GetFloat(const ID id)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
			return std::nullopt;

		const auto& value{ entry->get().value };
		if (!std::holds_alternative<float>(value))
			return std::nullopt;
		return std::get<float>(value);
	}

	std::optional<std::wstring> Property::GetString(const ID id)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
			return std::nullopt;

		const auto& value{ entry->get().value };
		if (!std::holds_alternative<std::wstring>(value))
			return std::nullopt;
		return std::get<std::wstring>(value);
	}

	std::optional<Sprite> Property::GetSprite(const ID id)
	{
		auto entry{ GetEntry(id) };
		if (!entry)
			return std::nullopt;

		const auto& value{ entry->get().value };
		if (!std::holds_alternative<Resource::Sprite>(value))
			return std::nullopt;
		return std::get<Resource::Sprite>(value);
	}

	void Unload(const ID id)
	{
		if (!g_hashes.contains(id))
			return;

		auto it{ std::ranges::lower_bound(g_entries, id, std::less{}, [](const auto& data) { return data.id; }) };
		if (it == g_entries.end())
			return;
		if (it->id != id)
			return;
		g_entries.erase(it);
		g_hashes.erase(id);
		g_children.erase(id);
	}

	bool Save(const ID id, const std::filesystem::path& path)
	{
		return ::Save(id, path);
	}
}
