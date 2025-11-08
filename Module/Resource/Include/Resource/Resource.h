#pragma once
#include "Export.h"
#include "Model.h"
#include "Property.h"
#include "Sprite.h"

namespace Resource
{
	struct Initializer
	{
		std::filesystem::path mountPath;
		std::function<Sprite(std::span<std::byte>)> loadSprite;
		std::function<std::shared_ptr<Model>(std::span<std::byte>)> loadModel;
	};

	RESOURCE_API void Initialize(const Initializer& initializer);
	RESOURCE_API void Uninitialize();
}
