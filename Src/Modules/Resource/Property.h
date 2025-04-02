#pragma once
#include <variant>

namespace Resource
{
	struct Model;
	class Property;
	class Sprite;
	class Texture;
}

template<class T>
concept is_property_data_type_v = 
	std::is_same_v<T, int32_t> ||
	std::is_same_v<T, Int2> ||
	std::is_same_v<T, float> ||
	std::is_same_v<T, std::wstring> ||
	std::is_same_v<T, std::shared_ptr<Resource::Sprite>> ||
	std::is_same_v<T, std::shared_ptr<Resource::Texture>> ||
	std::is_same_v<T, std::shared_ptr<Resource::Model>>;

namespace Resource
{
	class Property
	{
	public:
		enum class Type : int8_t
		{
			Folder,
			Int,
			Int2,
			Float,
			String,
			Sprite,
			Texture,
			Model
		};

		class Iterator
		{
		public:
			using iterator_concept = std::contiguous_iterator_tag;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = std::pair<const std::wstring, std::shared_ptr<Property>>;
			using difference_type = ptrdiff_t;
			using pointer = value_type*;
			using reference = std::pair<const std::wstring, std::shared_ptr<Property>&>;

		public:
			Iterator() = default;
			Iterator(std::vector<std::shared_ptr<Property>>::iterator iterator);
			~Iterator() = default;

			DLL_API reference operator*() const;
			DLL_API Iterator& operator++();
			DLL_API Iterator operator++(int);
			DLL_API Iterator& operator+=(const difference_type offset);
			DLL_API Iterator operator+(const difference_type offset);
			DLL_API Iterator& operator--();
			DLL_API Iterator operator--(int);
			DLL_API Iterator& operator-=(const difference_type offset);
			DLL_API Iterator operator-(const difference_type offset);
			DLL_API reference operator[](const difference_type offset);
			DLL_API bool operator==(const Iterator& rhs) const;
			DLL_API bool operator!=(const Iterator& rhs) const;

		private:
			std::vector<std::shared_ptr<Property>>::iterator m_iterator;
		};

	public:
		DLL_API Property();
		~Property() = default;

		DLL_API Iterator begin();
		DLL_API Iterator end();

		DLL_API void Add(const std::shared_ptr<Property>& child);
		DLL_API void Delete(const std::shared_ptr<Property>& child);

		DLL_API void SetType(Type type);
		DLL_API void SetName(std::wstring_view name);
		DLL_API void Set(int32_t value);
		DLL_API void Set(const Int2& value);
		DLL_API void Set(float value);
		DLL_API void Set(std::wstring_view value);
		DLL_API void Set(const std::shared_ptr<Sprite>& value);
		DLL_API void Set(const std::shared_ptr<Texture>& value);
		DLL_API void Set(const std::shared_ptr<Model>& value);

		DLL_API Type GetType() const;
		DLL_API std::wstring GetName() const;
		DLL_API int32_t GetInt(std::wstring_view path = L"") const;
		DLL_API Int2 GetInt2(std::wstring_view path = L"") const;
		DLL_API float GetFloat(std::wstring_view path = L"") const;
		DLL_API std::wstring GetString(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<Sprite> GetSprite(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<Texture> GetTexture(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<Model> GetModel(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<Property> Get(std::wstring_view path) const;
		DLL_API const std::vector<std::shared_ptr<Property>>& GetChildren() const;
		DLL_API std::vector<std::shared_ptr<Property>>& GetChildren();

#ifdef _TOOL
		DLL_API void SetParent(const std::shared_ptr<Property>& prop);
		DLL_API std::shared_ptr<Property> GetParent() const;
#endif

	private:
		std::wstring m_name;
		Type m_type;
		std::variant<
			int32_t,
			Int2,
			float,
			std::wstring,
			std::shared_ptr<Sprite>,
			std::shared_ptr<Texture>,
			std::shared_ptr<Model>
		> m_data;
		std::vector<std::shared_ptr<Property>> m_children;
#ifdef _TOOL
		std::weak_ptr<Property> m_parent;
#endif
	};
}
