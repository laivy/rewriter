#pragma once
#include "Common/Stdafx.h"

// Windows
#include <windowsx.h>
#include <commdlg.h>

// DirectX
#include <dxgi1_6.h>
#include <d3d12.h>
#include "External/DirectX/d3dx12.h"

// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_stdlib.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Module
#include "Module/Resource/Include/Image.h"
#include "Module/Resource/Include/Manager.h"
#include "Module/Resource/Include/Property.h"