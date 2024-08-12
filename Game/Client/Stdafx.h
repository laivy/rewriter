#pragma once
#include "Common/Stdafx.h"

// Windows
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <SDKDDKVer.h>
#include <wincodec.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// DirectX
#include <DirectXMath.h>
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dwrite.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include "External/DirectX/d3dx12.h"
#include "External/DirectX/WICTextureLoader12.h"

#ifdef _DEBUG
// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// Module
#include "Module/Resource/Include/Lib.h"

// Project
#include "DXUtil.h"
#include "Common/Packet.h"