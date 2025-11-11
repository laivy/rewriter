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

	GRAPHICS_API bool Begin();
	GRAPHICS_API bool End();
	GRAPHICS_API bool Present();
}
