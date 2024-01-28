#pragma once

template<class T>
concept is_property_data_type_v = 
	std::is_same_v<T, int32_t> ||
	std::is_same_v<T, INT2> ||
	std::is_same_v<T, float> ||
	std::is_same_v<T, std::wstring> ||
	std::is_same_v<T, std::shared_ptr<Resource::Image>>;

namespace Resource
{
	class Image;

	class Property
	{
	public:
		friend class Manager;

		enum class Type : unsigned char
		{
			FOLDER, INT, INT2, FLOAT, STRING, IMAGE
		};

		class Iterator
		{
		public:
			Iterator(const Property* const p, size_t index);
			~Iterator() = default;

			__declspec(dllexport) Iterator& operator++();
			__declspec(dllexport) Iterator& operator--();
			bool operator!=(const Iterator& it) const;
			std::pair<std::wstring, std::shared_ptr<Property>> operator*() const;

		private:
			const Property* const m_property;
			size_t m_childIndex;
		};

	public:
		Property();
		~Property() = default;

		Iterator begin() const;
		Iterator end() const;

		__declspec(dllexport) void Save(const std::filesystem::path& path);
		__declspec(dllexport) void Add(const std::shared_ptr<Property>& child);

		__declspec(dllexport) void SetType(Type type);
		__declspec(dllexport) void SetName(const std::wstring& name);
		__declspec(dllexport) void Set(int value);
		__declspec(dllexport) void Set(const INT2& value);
		__declspec(dllexport) void Set(float value);
		__declspec(dllexport) void Set(const std::wstring& value);
		__declspec(dllexport) void Set(const std::shared_ptr<Image>& value);

		__declspec(dllexport) Type GetType() const;
		__declspec(dllexport) std::wstring GetName() const;
		__declspec(dllexport) int GetInt() const;
		__declspec(dllexport) INT2 GetInt2() const;
		__declspec(dllexport) float GetFloat() const;
		__declspec(dllexport) std::wstring GetString() const;
		__declspec(dllexport) std::shared_ptr<Image> GetImage(const std::wstring& path = L"") const;
		__declspec(dllexport) std::shared_ptr<Property> Get(const std::wstring& path) const;

	private:
		// Manager에서 사용하는 함수들
		void Flush();

	private:
		Type m_type;
		std::wstring m_name;
		std::variant<
			int32_t,
			INT2,
			float,
			std::wstring,
			std::shared_ptr<Image>
		> m_data;
		std::vector<std::shared_ptr<Property>> m_children;
	};

	__declspec(dllexport) std::shared_ptr<Property> Load(const std::filesystem::path& path, const std::wstring& subPath = L"");
}