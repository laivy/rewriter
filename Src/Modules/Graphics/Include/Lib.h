#pragma once
#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllimport)

#include "../Graphics.h"
#include "../Graphics2D.h"
#include "../Graphics3D.h"
#ifdef _IMGUI
#include "../GraphicsImGui.h"
#endif
