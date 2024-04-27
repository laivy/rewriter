#pragma once
#include "Common/Stdafx.h"

// Windows
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <SDKDDKVer.h>
#include <wincodec.h>
#include <windowsx.h>
#include <winsock2.h>
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

// Resource
#include "Module/Resource/Include/Image.h"
#include "Module/Resource/Include/Manager.h"
#include "Module/Resource/Include/Property.h"

// Game
#include "DXUtil.h"
#include "Common/Event.h"
#include "Common/Packet.h"