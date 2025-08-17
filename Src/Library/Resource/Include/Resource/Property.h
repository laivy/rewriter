#pragma once
#include "Sprite.h"

namespace Resource::Property
{
	using ID = std::invoke_result_t<std::hash<std::wstring_view>, std::wstring_view>;

	// 직계 자식 프로퍼티 순회
	class Iterator
	{
	public:
		using value_type = std::pair<std::wstring_view, ID>;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;
		using difference_type = std::ptrdiff_t;

	public:
		Iterator(const ID id);
		Iterator(std::wstring_view path);

		bool operator!=(const Iterator& other) const;
		Iterator& operator++();
		value_type operator*() const;

		Iterator begin() const;
		Iterator end() const;

	private:
		ID m_parentID;
		std::size_t m_depth;
		std::size_t m_current;
		std::size_t m_end;
	};

	const ID InvalidID{ std::hash<std::wstring_view>{}(L"") };

	RESOURCE_API ID New(std::wstring_view name);
	RESOURCE_API ID New(const ID parentID, std::wstring_view name);
	RESOURCE_API ID Get(std::wstring_view path);
	RESOURCE_API ID Get(const ID parentID, std::wstring_view path);

	RESOURCE_API void Set(const ID id, std::int32_t value);
	RESOURCE_API void Set(const ID id, float value);
	RESOURCE_API void Set(const ID id, const std::wstring& value);
	RESOURCE_API void Set(const ID id, const Sprite& value);
	RESOURCE_API std::optional<std::int32_t> GetInt(const ID id);
	RESOURCE_API std::optional<float> GetFloat(const ID id);
	RESOURCE_API std::optional<std::wstring> GetString(const ID id);

	RESOURCE_API bool Save(const ID id, const std::filesystem::path& path);
}
