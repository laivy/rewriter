#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "imm32.lib")
#include <SDKDDKVer.h>
#include <windows.h>
#include <wincodec.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// C/C++
#include <any>
#include <array>
#include <cassert>
#include <chrono>
#include <codecvt>
#include <fstream>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <ranges>
#include <set>
#include <string>
#include <thread>
#include <variant>
#include <vector>

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
#include "d3dx12.h"
#include <dwrite.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// Game
#include "Common.h"
#include "Singleton.h"
#include "StringTable.h"
#include "TextUtil.h"
#include "Util.h"
#include "WICTextureLoader12.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// 전역 변수
extern UINT g_cbvSrvUavDescriptorIncrementSize;