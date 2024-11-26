#pragma once

namespace Resource
{
	class Property;
	class Sprite;

#if defined _CLIENT || defined _TOOL
	DLL_API void Initialize(const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& LoadSprite);
#endif
	DLL_API void CleanUp();

#ifdef _TOOL
	DLL_API bool Save(const std::shared_ptr<Property>& prop, std::wstring_view path);
#endif

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path);
	DLL_API void Unload(std::wstring_view path);
}
