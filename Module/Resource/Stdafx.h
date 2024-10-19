#pragma once
#include "Common/Stdafx.h"

#if defined _CLIENT || defined _TOOL
#include <d2d1_3.h>
#include <d3d12.h>
#include <wincodec.h>
#endif

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllexport)
