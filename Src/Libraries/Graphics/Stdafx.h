#pragma once
#include "Common/Stdafx.h"

// Windows
#include <wincodec.h>

// DirectX
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>

#ifdef _DIRECT2D
#include <d2d1_3.h>
#include <d3d11on12.h>
#endif

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include "External/DirectX/d3dx12.h"
#include "External/DirectX/WICTextureLoader12.h"

// ImGui
#ifdef _IMGUI
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// Project
#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllexport)
