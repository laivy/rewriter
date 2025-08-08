#pragma once
#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllimport)

#include "../Database.h"
#include "../Query.h"
