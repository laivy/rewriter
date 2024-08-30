#pragma once
#ifndef DLL_API
#define DLL_API __declspec(dllimport)
#endif // DLL_API

#include "../Graphics.h"
#include "../Graphics2D.h"
#include "../Graphics3D.h"
#ifdef _IMGUI
#include "../GraphicsImGui.h"
#endif // _IMGUI
