#pragma once
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include "../Query.h"

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllimport)
#include "../Database.h"
