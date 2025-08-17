#pragma once

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

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
