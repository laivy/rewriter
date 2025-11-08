#pragma once
#include "Define.h"
#include "Export.h"

namespace Resource
{
	// 직계 자식 프로퍼티 순회
	class Iterator
	{
	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = std::pair<std::wstring, ID>;
		using difference_type = std::ptrdiff_t;

	public:
		RESOURCE_API Iterator();
		RESOURCE_API Iterator(ID id);
		RESOURCE_API Iterator(const std::wstring& path);

		RESOURCE_API value_type operator*() const;
		RESOURCE_API Iterator& operator++();
		RESOURCE_API Iterator operator++(int);
		RESOURCE_API Iterator& operator--();
		RESOURCE_API Iterator operator--(int);
		RESOURCE_API Iterator& operator+=(const difference_type offset);
		RESOURCE_API Iterator operator+(const difference_type offset) const;
		RESOURCE_API Iterator& operator-=(const difference_type offset);
		RESOURCE_API Iterator operator-(const difference_type offset) const;
		RESOURCE_API difference_type operator-(const Iterator& other) const;
		RESOURCE_API value_type operator[](const difference_type offset) const;
		RESOURCE_API bool operator==(const Iterator& other) const;

		RESOURCE_API Iterator begin() const;
		RESOURCE_API Iterator end() const;

	private:
		ID m_parentID;
		std::size_t m_current;
		std::size_t m_end;
	};

	class RecursiveIterator
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<std::wstring, ID>;
		using difference_type = std::ptrdiff_t;

	public:
		RecursiveIterator() = default;
		RESOURCE_API RecursiveIterator(ID id);
		RESOURCE_API RecursiveIterator(const std::wstring& path);

		RESOURCE_API value_type operator*() const;
		RESOURCE_API RecursiveIterator& operator++();
		RESOURCE_API bool operator==(const RecursiveIterator& other) const;

		RESOURCE_API RecursiveIterator begin() const;
		RESOURCE_API RecursiveIterator end() const;

	private:
		std::vector<Iterator> m_stacks;
	};

	RESOURCE_API ID New(const std::wstring& name);
	RESOURCE_API ID New(ID parentID, const std::wstring& name);
	RESOURCE_API void Delete(ID id);
	RESOURCE_API ID Get(const std::wstring& path);
	RESOURCE_API ID Get(ID parentID, const std::wstring& path);
	RESOURCE_API ID GetParent(ID id);
	RESOURCE_API std::size_t Size(ID id);
	RESOURCE_API void Move(ID targetID, ID parentID, std::optional<std::size_t> index = std::nullopt);

	RESOURCE_API bool SetName(ID id, const std::wstring& name);
	RESOURCE_API void Set(ID id, std::monostate value);
	RESOURCE_API void Set(ID id, std::int32_t value);
	RESOURCE_API void Set(ID id, float value);
	RESOURCE_API void Set(ID id, const std::wstring& value);
	//RESOURCE_API void Set(const ID id, const Sprite& value);

	RESOURCE_API std::optional<std::wstring> GetName(ID id);
	RESOURCE_API std::optional<std::wstring> GetName(ID id, const std::wstring& path);
	RESOURCE_API std::optional<std::int32_t> GetInt(const ID id);
	RESOURCE_API std::optional<std::int32_t> GetInt(const ID id, const std::wstring& path);
	RESOURCE_API std::optional<float> GetFloat(const ID id);
	RESOURCE_API std::optional<float> GetFloat(const ID id, const std::wstring& path);
	RESOURCE_API std::optional<std::wstring> GetString(const ID id);
	RESOURCE_API std::optional<std::wstring> GetString(const ID id, const std::wstring& path);
	//RESOURCE_API std::optional<Sprite> GetSprite(const ID id);

	//RESOURCE_API void Unload(const ID id);
	RESOURCE_API bool SaveToFile(const ID id, const std::filesystem::path& path);
}
