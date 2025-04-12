#pragma once

namespace Resource
{
	struct Model;
	class Property;
	class Sprite;
	class Texture;

#if defined _CLIENT || defined _TOOL
	DLL_API void Initialize(
		const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& loadSprite,
		const std::function<std::shared_ptr<Texture>(std::span<std::byte>)>& loadTexture,
		const std::function<std::shared_ptr<Model>(std::span<std::byte>)>& loadModel
	);
#endif
	DLL_API void CleanUp();

#ifdef _TOOL
	DLL_API bool Save(const std::shared_ptr<Property>& prop, std::wstring_view path);
#endif

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path);
	DLL_API void Unload(std::wstring_view path);
}
