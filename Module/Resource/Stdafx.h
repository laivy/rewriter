#pragma once

// DLL
#define DLLEXPORT __declspec(dllexport)

// Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// C++
#include <array>
#include <cassert>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

// Game
#include "Game/Common/Singleton.h"
#include "Game/Common/Types.h"