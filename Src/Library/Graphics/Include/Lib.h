#pragma once

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

#include "../Camera.h"
#include "../Graphics.h"
#include "../Graphics2D.h"
#include "../Graphics3D.h"
#ifdef _IMGUI
#include "../GraphicsImGui.h"
#endif
