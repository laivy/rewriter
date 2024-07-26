#pragma once
#include <variant>

namespace Resource
{
	class PNG;
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
		enum class Type : unsigned char
		{
			FOLDER, INT, INT2, FLOAT, STRING, PNG, DDS
		};

		class Iterator
		{
		public:
			Iterator(const Property* const prop, size_t index);
			~Iterator() = default;

			__declspec(dllexport) Iterator& operator++();
			__declspec(dllexport) Iterator& operator--();
			__declspec(dllexport) bool operator==(const Iterator& iter) const;
			__declspec(dllexport) bool operator!=(const Iterator& iter) const;
			__declspec(dllexport) std::pair<std::wstring, std::shared_ptr<Property>> operator*() const;

		private:
			const Property* const m_property;
			size_t m_index;
		};

	public:
		__declspec(dllexport) Property();
		~Property() = default;

		__declspec(dllexport) Iterator begin() const;
		__declspec(dllexport) Iterator end() const;

#ifdef _TOOL
		__declspec(dllexport) void Save(const std::filesystem::path& path);
		__declspec(dllexport) void Add(const std::shared_ptr<Property>& child);
#endif

		__declspec(dllexport) void SetType(Type type);
		__declspec(dllexport) void SetName(const std::wstring& name);
		__declspec(dllexport) void Set(int value);
		__declspec(dllexport) void Set(const INT2& value);
		__declspec(dllexport) void Set(float value);
		__declspec(dllexport) void Set(const std::wstring& value);
		__declspec(dllexport) void Set(const std::shared_ptr<PNG>& value);

		__declspec(dllexport) Type GetType() const;
		__declspec(dllexport) std::wstring GetName() const;
		__declspec(dllexport) int GetInt(std::wstring_view path = L"") const;
		__declspec(dllexport) INT2 GetInt2(std::wstring_view path = L"") const;
		__declspec(dllexport) float GetFloat(std::wstring_view path = L"") const;
		__declspec(dllexport) std::wstring GetString(std::wstring_view path = L"") const;
		__declspec(dllexport) std::shared_ptr<PNG> GetImage(std::wstring_view path = L"") const;
		__declspec(dllexport) std::shared_ptr<Property> Get(std::wstring_view path) const;

		void Erase(std::wstring_view path);

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