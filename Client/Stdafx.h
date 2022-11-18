#pragma once

#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "Shcore.lib")
#include <SDKDDKVer.h>
#include <windows.h>
#include <shellscalingapi.h>
#include <wrl.h>
#include <array>
#include <cassert>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <ranges>
#include <thread>
#include <mutex>
#include <functional>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "Singleton.h"
#include "Util.h"
#include "Defines.h"

using Microsoft::WRL::ComPtr;

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif