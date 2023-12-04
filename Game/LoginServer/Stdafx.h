#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// C/C++
#include <atlstr.h>
#include <array>
#include <format>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

// DirectX
#include <dxgi1_6.h>
#include <d3d12.h>
#include "External/DirectX/d3dx12.h"
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Database
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "Module/Database/Include/Types.h"
#include "Module/Database/Include/Connection.h"
#include "Module/Database/Include/Login.h"

// Game
#include "Common/Network.h"
#include "Common/Packet.h"
#include "Common/Request.h"
#include "Common/Singleton.h"
#include "Common/Types.h"