#pragma once
#include <span>
#include <Resource/Resource.h>
#include "Export.h"
#include "ImGui.h"

namespace Graphics
{
	GRAPHICS_API bool Initialize(void* hWnd);
	GRAPHICS_API void Finalize();

	GRAPHICS_API Resource::Sprite LoadSprite(std::span<char> binary);

	GRAPHICS_API bool Begin3D();
	GRAPHICS_API bool End3D();
	GRAPHICS_API bool Begin2D();
	GRAPHICS_API bool End2D();
	GRAPHICS_API bool Present();
}
