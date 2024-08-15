#pragma once
#include "Common/Stdafx.h"

// Windows
#include <WinSock2.h> // MSWSock.h 보다 위에 있어야함
#include <MSWSock.h>
#include <WS2tcpip.h>

// Imgui
#include "External/Imgui/imgui.h"
#include "External/Imgui/imgui_internal.h"
#include "External/Imgui/imgui_impl_win32.h"
#include "External/Imgui/imgui_impl_dx12.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Module
#include "Module/Resource/Include/Lib.h"

// Project
#include "Common/Packet.h"
