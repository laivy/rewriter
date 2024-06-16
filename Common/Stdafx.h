#pragma once

// 이 헤더 파일은 모든 프로젝트의 Stdafx.h에서 가장 첫번째로 포함한다.
// 모든 프로젝트에서 공통으로 사용하는 헤더들을 포함한다.

// Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;
#ifdef DrawText
#undef DrawText
#endif

// C/C++
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <codecvt>
#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <thread>
#include <unordered_map>
#include <variant>
#include <vector>
using namespace std::chrono_literals;

// Project
#include "Common/Event.h"
#include "Common/Singleton.h"
#include "Common/StringTable.h"
#include "Common/Types.h"