export module Library.Resource;

export import :Model;
export import :Property;
export import :Sprite;

import std;

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

export namespace Resource
{
	RESOURCE_API void Initialize(
		const std::filesystem::path& mountPath,
		const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& loadSprite,
		const std::function<std::shared_ptr<Model>(std::span<std::byte>)>& loadModel
	);

	RESOURCE_API void CleanUp();
}
