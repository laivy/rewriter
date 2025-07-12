export module Library.Resource:Property;

import std;
import Common.Type;

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

export namespace Resource
{
	struct Property;
	struct Sprite;

	RESOURCE_API std::shared_ptr<Property> Get(std::wstring_view path);
	RESOURCE_API std::shared_ptr<Property> Get(const std::shared_ptr<Property>& prop, std::wstring_view path);
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
	RESOURCE_API bool Save(const std::filesystem::path& filePath, const std::shared_ptr<Property>& prop);
#endif
}
