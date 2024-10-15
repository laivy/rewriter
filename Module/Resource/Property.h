#pragma once
#include <variant>

namespace Resource
{
	class PNG;
	class Property;
}

template<class T>
concept is_property_data_type_v = 
	std::is_same_v<T, int32_t> ||
	std::is_same_v<T, INT2> ||
	std::is_same_v<T, float> ||
	std::is_same_v<T, std::wstring> ||
	std::is_same_v<T, std::shared_ptr<Resource::PNG>>;

namespace Resource
{
	class Property
	{
	public:
		enum class Type : uint8_t
		{
			Folder,
			Int,
			Int2,
			Float,
			String,
			Image
		};

	public:
		DLL_API Property();
		~Property() = default;

		DLL_API void Add(const std::shared_ptr<Property>& child);
		DLL_API void Delete(const std::shared_ptr<Property>& child);

		DLL_API void SetType(Type type);
		DLL_API void SetName(const std::wstring& name);
		DLL_API void Set(int32_t value);
		DLL_API void Set(const INT2& value);
		DLL_API void Set(float value);
		DLL_API void Set(const std::wstring& value);
		DLL_API void Set(const std::shared_ptr<PNG>& value);

		DLL_API Type GetType() const;
		DLL_API std::wstring GetName() const;
		DLL_API int32_t GetInt(std::wstring_view path = L"") const;
		DLL_API INT2 GetInt2(std::wstring_view path = L"") const;
		DLL_API float GetFloat(std::wstring_view path = L"") const;
		DLL_API std::wstring GetString(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<PNG> GetImage(std::wstring_view path = L"") const;
		DLL_API std::shared_ptr<Property> Get(std::wstring_view path) const;
		DLL_API const std::vector<std::shared_ptr<Property>>& GetChildren() const;
		DLL_API std::vector<std::shared_ptr<Property>>& GetChildren();

	private:
		std::wstring m_name;
		Type m_type;
		std::variant<
			int32_t,
			INT2,
			float,
			std::wstring,
			std::shared_ptr<PNG>
		> m_data;
		std::vector<std::shared_ptr<Property>> m_children;
	};
}
