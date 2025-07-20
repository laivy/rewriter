export module rewriter.library.resource:property;

import std;
import rewriter.common.type;

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

export namespace Resource
{
	struct Property;
	struct Sprite;

	class Iterator
	{
	public:
		using Container = std::vector<std::shared_ptr<Property>>;

	public:
		using iterator_concept = Container::iterator::iterator_concept;
		using iterator_category = Container::iterator::iterator_category;
		using value_type = std::pair<const std::wstring, std::shared_ptr<Property>>;
		using difference_type = Container::difference_type;
		using reference = std::pair<const std::wstring, std::shared_ptr<Property>&>;

	public:
		Iterator() = default;
		RESOURCE_API Iterator(Container::iterator iterator);

		RESOURCE_API reference operator*() const;
		RESOURCE_API Iterator& operator++();
		RESOURCE_API Iterator operator++(int);
		RESOURCE_API Iterator& operator+=(const difference_type offset);
		RESOURCE_API Iterator operator+(const difference_type offset);
		RESOURCE_API Iterator& operator--();
		RESOURCE_API Iterator operator--(int);
		RESOURCE_API Iterator& operator-=(const difference_type offset);
		RESOURCE_API Iterator operator-(const difference_type offset);
		RESOURCE_API reference operator[](const difference_type offset);
		RESOURCE_API bool operator==(const Iterator& rhs) const;
		RESOURCE_API bool operator!=(const Iterator& rhs) const;

	private:
		Container::iterator m_iterator;
	};

	class View : public std::ranges::view_interface<View>
	{
	public:
		using Container = std::vector<std::shared_ptr<Property>>;

	public:
		View(Container::iterator begin, Container::iterator end);

		RESOURCE_API Iterator begin() const;
		RESOURCE_API Iterator end() const;
		RESOURCE_API size_t size() const;

	private:
		Container::iterator m_begin;
		Container::iterator m_end;
	};

	RESOURCE_API std::shared_ptr<Property> Get(std::wstring_view path);
	RESOURCE_API std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, std::wstring_view path);
	RESOURCE_API std::wstring GetName(std::wstring_view path);
	RESOURCE_API std::wstring GetName(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API std::int32_t GetInt(std::wstring_view path);
	RESOURCE_API std::int32_t GetInt(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API Int2 GetInt2(std::wstring_view path);
	RESOURCE_API Int2 GetInt2(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API float GetFloat(std::wstring_view path);
	RESOURCE_API float GetFloat(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API std::wstring GetString(std::wstring_view path);
	RESOURCE_API std::wstring GetString(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API std::shared_ptr<Sprite> GetSprite(std::wstring_view path);
	RESOURCE_API std::shared_ptr<Sprite> GetSprite(const std::shared_ptr<Property>& prop, std::wstring_view path = L"");
	RESOURCE_API View Iterate(const std::shared_ptr<Property>& prop);
	RESOURCE_API View Iterate(std::wstring_view path);
	RESOURCE_API void Unload(std::wstring_view path);

#ifdef _TOOL
	RESOURCE_API std::shared_ptr<Property> NewProperty();
	RESOURCE_API void SetName(const std::shared_ptr<Property>& prop, std::wstring_view name);
	RESOURCE_API void AddChild(const std::shared_ptr<Property>& prop, const std::shared_ptr<Property>& child);
	RESOURCE_API bool Save(const std::filesystem::path& filePath, const std::shared_ptr<Property>& prop);
#endif
}
