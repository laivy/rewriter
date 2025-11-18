#pragma once
// C++ 표준 라이브러리
#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <ranges>
#include <vector>
#include <unordered_map>

// Windows
#include <wincodec.h>
#include <wrl/client.h>

// DirectX
#include <directx/d3dx12.h> // 가장 먼저 포함해야 함
#include <d2d1_3.h>
#include <d3d11on12.h>
#include <dwrite_3.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#include <WICTextureLoader/WICTextureLoader12.h>

// ImGui
#ifdef _IMGUI
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <misc/cpp/imgui_stdlib.h>
#endif

// 프로젝트
#include <Common/Singleton.h>
#include <Resource/Resource.h>
