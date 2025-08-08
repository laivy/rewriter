#pragma once
#include "Common/Stdafx.h"

// C/C++
#include <format>

// SQL
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

// Library
#include "Library/Resource/Include/Lib.h"

// Project
#include "Types.h"

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllexport)
