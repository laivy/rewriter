#pragma once

namespace Resource
{
	struct Sprite;
	struct Model;

	RESOURCE_API void Initialize(
		const std::filesystem::path& mountPath,
		const std::function<std::shared_ptr<Sprite>(std::span<std::byte>)>& loadSprite,
		const std::function<std::shared_ptr<Model>(std::span<std::byte>)>& loadModel
	);

	RESOURCE_API void CleanUp();
}
