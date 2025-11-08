#pragma once
#include "Common/Pch.h"

// Windows
#include <wincodec.h>

// DirectX
#include <directx/d3dx12.h> // 가장 먼저 포함해야 함
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

// ImGui
#ifdef _IMGUI
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// Project Library
#include <Resource/Resource.h>
