#pragma once
// C++ 표준 라이브러리
#include <span>

// 프로젝트 모듈
#include <Resource/Resource.h>

// 프로젝트
#include "Export.h"
#include "ImGui.h"
#include "ImGuiFileDialog.h"

namespace Graphics
{
	GRAPHICS_API bool Initialize(void* hWnd);
	GRAPHICS_API void Finalize();

	GRAPHICS_API Resource::Sprite LoadSprite(Resource::ID id, std::span<std::byte> binary);

	GRAPHICS_API bool Begin3D();
	GRAPHICS_API bool End3D();
	GRAPHICS_API bool Begin2D();
	GRAPHICS_API bool End2D();
	GRAPHICS_API bool Present();
}
