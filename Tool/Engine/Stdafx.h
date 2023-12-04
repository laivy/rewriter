#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// C, C++
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>
#include <set>
#include <unordered_map>

// DirectX
#include <dxgi1_6.h>
#include <d3d12.h>
#include "External/DirectX/d3dx12.h"

// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Module
#include "Module/Resource/Include/Image.h"
#include "Module/Resource/Include/Property.h"
#include "Module/Resource/Include/ResourceManager.h"

// Project
#include "Common/Singleton.h"
#include "Common/Types.h"