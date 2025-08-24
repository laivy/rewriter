#pragma once
#include "Sprite.h"

namespace Resource::Property
{
	using ID = std::invoke_result_t<std::hash<std::wstring_view>, std::wstring_view>;

	// 직계 자식 프로퍼티 순회
	class RESOURCE_API Iterator
	{
	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = std::pair<std::wstring, ID>;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

	public:
		Iterator();
		Iterator(const ID id);
		Iterator(std::wstring_view path);

		value_type operator*() const;
		Iterator& operator++();
		Iterator operator++(int);
		Iterator& operator--();
		Iterator operator--(int);
		Iterator& operator+=(const difference_type offset);
		Iterator operator+(const difference_type offset) const;
		Iterator& operator-=(const difference_type offset);
		Iterator operator-(const difference_type offset) const;
		difference_type operator-(const Iterator& other) const;
		value_type operator[](const difference_type offset) const;
		bool operator==(const Iterator& other) const;

		Iterator begin() const;
		Iterator end() const;

	private:
		ID m_parentID;
		std::size_t m_current;
		std::size_t m_end;
	};

	const ID InvalidID{ std::hash<std::wstring_view>{}(L"") };

	RESOURCE_API ID New(std::wstring_view name);
	RESOURCE_API ID New(const ID parentID, std::wstring_view name);
	RESOURCE_API ID Get(std::wstring_view path);
	RESOURCE_API ID Get(const ID parentID, std::wstring_view path);
	RESOURCE_API ID GetParent(const ID id);
	RESOURCE_API std::wstring GetPath(const ID id);

	RESOURCE_API void SetName(const ID id, std::wstring_view name);
	RESOURCE_API void Set(const ID id, std::int32_t value);
	RESOURCE_API void Set(const ID id, float value);
	RESOURCE_API void Set(const ID id, std::wstring_view value);
	RESOURCE_API void Set(const ID id, const Sprite& value);
	RESOURCE_API std::optional<std::wstring> GetName(const ID id);
	RESOURCE_API std::optional<std::int32_t> GetInt(const ID id);
	RESOURCE_API std::optional<float> GetFloat(const ID id);
	RESOURCE_API std::optional<std::wstring> GetString(const ID id);
	RESOURCE_API std::optional<Sprite> GetSprite(const ID id);

	RESOURCE_API void Unload(const ID id);
	RESOURCE_API bool Save(const ID id, const std::filesystem::path& path);
}
