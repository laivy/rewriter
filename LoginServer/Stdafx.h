#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>
#include <wrl.h>
#pragma comment(lib, "MSWSock.lib")
#pragma comment(lib, "ws2_32.lib")
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
#include <string>
#include <thread>
#include <unordered_map>

// DirectX
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#include <d3d12.h>
#include "../DirectX/d3dx12.h"
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// Imgui
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_win32.h"
#include "../Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Game
#include "../Common/Network.h"
#include "../Common/Packet.h"
#include "../Common/Request.h"
#include "../Common/Singleton.h"
#include "../Common/Types.h"

// Database
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/Database.lib")
#else
#pragma comment(lib, "../x64/Release/Database.lib")
#endif
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "../Database/Include/Connection.h"