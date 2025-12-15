#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <span>
#include <Resource/Resource.h>
#include <Common/Type.h>
#include "Export.h"
#include "Model.h"
#include "Property.h"
#include "Sprite.h"

namespace Resource
{
	struct Initializer
	{
		std::filesystem::path mountPath;
		std::function<Sprite(ID, std::span<std::byte>)> loadSprite;
	};

	RESOURCE_API void Initialize(const Initializer& initializer);
	RESOURCE_API void Finalize();
}
