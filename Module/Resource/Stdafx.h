#pragma once

// DLL
#define DLLEXPORT __declspec(dllexport)

// Windows
#define WIN32_LEAN_AND_MEAN
#include <wincodec.h>
#include <Windows.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

// C++
#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

// DirectX
#pragma comment(lib, "d2d1.lib")
#include <d2d1_3.h>
#include <d3d12.h>

// Game
#include "Game/Common/Singleton.h"
#include "Game/Common/Types.h"