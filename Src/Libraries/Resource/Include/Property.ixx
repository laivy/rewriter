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

		using iterator_concept = Container::iterator::iterator_concept;
		using iterator_category = Container::iterator::iterator_category;
		using value_type = std::pair<std::wstring, std::shared_ptr<Property>>;
		using difference_type = Container::difference_type;
		using reference = std::pair<std::wstring, std::shared_ptr<Property>&>;

	public:
		RESOURCE_API Iterator();
		RESOURCE_API Iterator(std::wstring_view path);
		RESOURCE_API Iterator(const std::shared_ptr<Property>& prop);

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

		RESOURCE_API Iterator begin() const;
		RESOURCE_API Iterator end() const;

	private:
		Property* m_prop;
		Container::iterator m_iter;
	};

	class RecursiveIterator
	{
	public:
		using Container = std::vector<std::shared_ptr<Property>>;

		using iterator_category = std::input_iterator_tag;
		using value_type = std::pair<std::wstring, std::shared_ptr<Property>>;
		using difference_type = Container::difference_type;
		using reference = std::pair<std::wstring, std::shared_ptr<Property>&>;

	public:
		RESOURCE_API RecursiveIterator();
		RESOURCE_API RecursiveIterator(std::wstring_view path);
		RESOURCE_API RecursiveIterator(const std::shared_ptr<Property>& prop);

		RESOURCE_API RecursiveIterator& operator++();
		RESOURCE_API RecursiveIterator operator++(int);
		RESOURCE_API bool operator==(const RecursiveIterator& rhs) const;
		RESOURCE_API bool operator!=(const RecursiveIterator& rhs) const;
		RESOURCE_API reference operator*() const;

		RESOURCE_API RecursiveIterator begin() const;
		RESOURCE_API RecursiveIterator end() const;

	private:
		Property* m_prop;
		Container::iterator m_iter;
		std::vector<Container::iterator> m_parents;
		std::vector<Container::iterator> m_ends;
	};

	RESOURCE_API std::shared_ptr<Property> Get(std::wstring_view path);
	RESOURCE_API std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, std::wstring_view path);
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
	RESOURCE_API void Unload(std::wstring_view path);

#ifdef _TOOL
	RESOURCE_API std::shared_ptr<Property> NewProperty();
	RESOURCE_API void SetName(const std::shared_ptr<Property>& prop, std::wstring_view name);
	RESOURCE_API void AddChild(const std::shared_ptr<Property>& prop, const std::shared_ptr<Property>& child);
	RESOURCE_API bool Save(const std::filesystem::path& filePath, const std::shared_ptr<Property>& prop);
#endif
}
