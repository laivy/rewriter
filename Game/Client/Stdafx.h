#pragma once

// DLL
#define DLLEXPORT __declspec(dllexport)

// Windows
#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <SDKDDKVer.h>
#include <wincodec.h>
#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>
#include <wrl.h>
#include <WS2tcpip.h>
using Microsoft::WRL::ComPtr;

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// C/C++
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <ranges>
#include <string>
#include <thread>
#include <variant>
#include <vector>
using namespace std::chrono_literals;

// DirectX
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")
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

// Game
#include "Game/Common/Packet.h"
#include "Game/Common/Singleton.h"
#include "Game/Common/StringTable.h"
#include "Game/Common/Types.h"
#include "DXUtil.h"
#include "TextUtil.h"

// Resource
#include "Module/Resource/Include/Image.h"
#include "Module/Resource/Include/Property.h"
#include "Module/Resource/Include/ResourceManager.h"

// 전역 변수
extern UINT g_cbvSrvUavDescriptorIncrementSize;

enum class RootParamIndex : unsigned int
{
	// 상수버퍼
	GAMEOBJECT,
	CAMERA,
	TEXTURE,
#ifdef _DEBUG
	LINE,
#endif

	// 서술자
	TEXTURE0,

	// 루트파라미터 개수
	COUNT
};

enum class Layer
{
	// 먼저 선언되있을 수록 위에 그려짐
	LOCALPLAYER,
	REMOTEPLAYER,
	MONSTER,
	TERRAIN,
	BACKGROUND1,
	BACKGROUND2,
	BACKGROUND3,
	COUNT
};

enum class Pivot
{
	LEFTTOP, CENTERTOP, RIGHTTOP,
	LEFTCENTER, CENTER, RIGHTCENTER,
	LEFTBOT, CENTERBOT, RIGHTBOT
};
