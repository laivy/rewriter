#pragma once
#include "Shared/Stdafx.h"

// Windows
#include <wincodec.h>

// DirectX
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG
#include "External/DirectX/d3dx12.h"
#include "External/DirectX/WICTextureLoader12.h"

// ImGui
#ifdef _IMGUI
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// Module
#include "Modules/Resource/Include/Lib.h"

// Project
#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllexport)
