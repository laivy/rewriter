#pragma once
#include "Common/Types.h"

template<class T>
concept is_property_data_type_v = std::is_same_v<T, int32_t> || std::is_same_v<T, INT2> || std::is_same_v<T, float> || std::is_same_v<T, std::string> || std::is_same_v<T, std::shared_ptr<Resource::Image>>;

namespace Resource
{
	class Image;

	class Property
	{
	public:
		friend class ResourceManager;

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
			__declspec(dllexport) bool operator!=(const Iterator& it) const;
			__declspec(dllexport) std::pair<std::string, std::shared_ptr<Property>> operator*() const;

		private:
			const Property* const m_property;
			size_t m_childIndex;
		};

	public:
		Property();
		~Property();

		__declspec(dllexport) Iterator begin() const;
		__declspec(dllexport) Iterator end() const;

		void Save(std::ostream& file);
		void Load(std::istream& file);

		void SetType(Type type);
		void SetName(const std::string& name);
		void Set(int data);
		void Set(const INT2& data);
		void Set(float data);
		void Set(const std::string& data);
		void Set(const std::shared_ptr<Image>& data);

		Type GetType() const;
		std::string GetName() const;
		int GetInt() const;
		INT2 GetInt2() const;
		float GetFloat() const;
		std::string GetString() const;
		std::shared_ptr<Image> GetImage() const;
		std::shared_ptr<Property> Get(const std::string& path) const;

	private:
		// ResourceManager에서 사용하는 함수들
		void Load(std::istream& file, std::string& name);
		void Flush();

	private:
		Type m_type;
		std::string m_name;
		std::variant<
			int32_t,
			INT2,
			float,
			std::string,
			std::shared_ptr<Image>
		> m_data;
		std::vector<std::shared_ptr<Property>> m_children;
	};

	__declspec(dllexport) Property::Type GetType(const std::shared_ptr<Property>& prop);
	__declspec(dllexport) std::string GetName(const std::shared_ptr<Property>& prop);
	__declspec(dllexport) int32_t GetInt(const std::shared_ptr<Property>& prop, const std::string& path = "");
	__declspec(dllexport) INT2 GetInt2(const std::shared_ptr<Property>& prop, const std::string& path = "");
	__declspec(dllexport) float GetFloat(const std::shared_ptr<Property>& prop, const std::string& path = "");
	__declspec(dllexport) std::string GetString(const std::shared_ptr<Property>& prop, const std::string& path = "");
	__declspec(dllexport) std::shared_ptr<Image> GetImage(const std::shared_ptr<Property>& prop, const std::string& path = "");

	__declspec(dllexport) std::shared_ptr<Property> Create();
	__declspec(dllexport) void SetType(const std::shared_ptr<Property>& prop, Property::Type type);
	__declspec(dllexport) void SetName(const std::shared_ptr<Property>& prop, const std::string& value);
	__declspec(dllexport) void Set(const std::shared_ptr<Property>& prop, int32_t value);
	__declspec(dllexport) void Set(const std::shared_ptr<Property>& prop, const INT2& value);
	__declspec(dllexport) void Set(const std::shared_ptr<Property>& prop, float value);
	__declspec(dllexport) void Set(const std::shared_ptr<Property>& prop, const std::string& value);
	__declspec(dllexport) void Set(const std::shared_ptr<Property>& prop, const std::shared_ptr<Image>& value);

#ifdef _TOOL
	__declspec(dllexport) void Save(const std::shared_ptr<Property>& prop, std::ostream& file);
	__declspec(dllexport) std::shared_ptr<Property> Load(std::istream& file);
#endif
}