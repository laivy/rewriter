#pragma once
#include "Common/Stdafx.h"

// Windows
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <WinSock2.h>

// C/C++
#include <atlstr.h>
#include <execution>
#include <format>
#include <semaphore>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include "External/DirectX/d3dx12.h"

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

// Project
#include "Common/Packet.h"
#include "Common/Request.h"
#include "Common/Singleton.h"
#include "Common/Types.h"