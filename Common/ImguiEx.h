#pragma once

// C/C++
#include <string>

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

// ImGui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_impl_dx12.h"
#include "External/Imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
