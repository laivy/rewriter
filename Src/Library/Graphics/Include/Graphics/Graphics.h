#pragma once

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

#include "Camera.h"
#include "Graphics.h"
#include "Graphics2D.h"
#include "Graphics3D.h"
#ifdef _IMGUI
#include "GraphicsImGui.h"
#endif

namespace Graphics
{
	GRAPHICS_API bool Initialize(void* hWnd);
	GRAPHICS_API void Uninitialize();

	GRAPHICS_API void OnWindowResized(int width, int height);
}
