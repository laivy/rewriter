#pragma once
#include "Resource.h"

namespace Resource
{
	class Manager :
		public IObserver,
		public TSingleton<Manager>
	{
	private:
		static constexpr std::string_view Signature{ "lvy" };

		using Value = std::variant<
			std::monostate,
			std::int32_t,
			float,
			std::wstring
		>;

		struct Property
		{
			std::wstring name;
			Value value;
		};

		struct Entry
		{
			std::wstring path;
			ID parentID;
			std::vector<ID> children;
		};

	public:
		Manager();

		ID New(const std::wstring& path);
		ID New(ID parentID, const std::wstring& path);
		void Delete(ID id);
		ID Get(const std::wstring& path) const;
		ID Get(ID parentID, const std::wstring& path) const;
		ID GetParent(ID id) const;
		ID GetChild(ID parentID, std::size_t index) const;
		std::size_t GetChildCount(ID parentID) const;

		void SetName(ID id, const std::wstring& name);

		template<class T>
		requires std::is_constructible_v<Value, T>
		void Set(ID id, T value)
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
			prop->value = value;
		}

		std::wstring GetName(ID id) const;

		template<class T>
		requires std::is_constructible_v<Value, T>
		std::optional<T> Get(ID id) const
		{
			if (id >= m_properties.size())
			{
				assert(false && "invalid id");
				return std::nullopt;
			}
			const auto& prop{ m_properties.at(id) };
			if (!prop)
			{
				assert(false && "not exists");
				return std::nullopt;
			}
			if (!std::holds_alternative<T>(prop->value))
			{
				assert(false && "invalid type");
				return std::nullopt;
			}
			return std::get<T>(prop->value);
		}

		bool SaveToFile(ID id, const std::filesystem::path& path) const;

	private:
		void OnInitialize(const Initializer& initializer);
		void OnUninitialize();

		ID LoadFromFile(const std::filesystem::path& path, const std::wstring& subPath);

	private:
		std::filesystem::path m_mountPath;
		std::function<Sprite(std::span<std::byte>)> m_loadSprite;
		std::function<std::shared_ptr<Model>(std::span<std::byte>)> m_loadModel;

		std::vector<std::optional<Property>> m_properties;
		std::unordered_map<std::wstring, ID> m_pathToID;
		std::unordered_map<ID, Entry> m_idToEntry;
	};
}
