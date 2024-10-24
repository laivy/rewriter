#pragma once
#include "Common/Stdafx.h"

// C/C++
#include <set>

// Windows
#include <commdlg.h>
#include <windowsx.h>

// DirectX
#include <d2d1_3.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "External/DirectX/d3dx12.h"

// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_stdlib.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Module
#include "Module/Graphics/Include/Lib.h"
#include "Module/Resource/Include/Lib.h"
