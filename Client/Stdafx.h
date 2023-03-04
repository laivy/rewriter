#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN 
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
#include <fstream>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <ranges>
#include <set>
#include <string>
#include <thread>
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

// Nyaight
#include "Common.h"
#include "NytDataType.h"
#include "Singleton.h"
#include "StringTable.h"
#include "Util.h"
#include "WICTextureLoader12.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// 전역 변수
extern UINT g_cbvSrvUavDescriptorIncrementSize;

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result{ hr }
		{
			OutputDebugStringA(what());
		}

		const char* what() const override
		{
			static char s_str[64]{};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<UINT>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
			throw com_exception{ hr };
	}
}