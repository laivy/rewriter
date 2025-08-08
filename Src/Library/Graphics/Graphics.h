#pragma once

namespace Graphics
{
	GRAPHICS_API bool Initialize(void* hWnd);
	GRAPHICS_API void CleanUp();

	GRAPHICS_API void OnWindowResized(int width, int height);
}
