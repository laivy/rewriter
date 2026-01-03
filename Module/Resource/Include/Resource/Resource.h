#pragma once
// C++ 표준 라이브러리
#include <cstddef>
#include <filesystem>
#include <span>

// 프로젝트 모듈
#include <Resource/Resource.h>

// 프로젝트
#include <Common/Type.h>
#include "Export.h"
#include "Model.h"
#include "Property.h"
#include "Sprite.h"

namespace Resource
{
	using LoadSpriteFunc = Sprite(*)(std::span<std::byte>);

	struct Initializer
	{
		std::filesystem::path mountPath;
		LoadSpriteFunc loadSprite;
	};

	RESOURCE_API void Initialize(const Initializer& initializer);
	RESOURCE_API void Finalize();
}
