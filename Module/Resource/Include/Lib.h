#pragma once
#ifdef DLL_API
#undef DLL_API
#endif // DLL_API
#define DLL_API __declspec(dllimport)

#include "../Property.h"
#include "../Resource.h"
#include "../Sprite.h"
