#pragma once

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
#include <variant>

// DirectX
#include <d2d1_3.h>
#include <d3d12.h>

// Game
#include "Common/Singleton.h"
#include "Common/Types.h"