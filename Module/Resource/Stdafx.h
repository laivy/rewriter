#pragma once
#include "Common/Stdafx.h"

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllexport)
