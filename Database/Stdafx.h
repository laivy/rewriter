#pragma once

// DLL
#define DLLEXPORT __declspec(dllexport)

// Windows
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// C/C++
#include <format>
#include <memory>
#include <string>

// SQL
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
using SPRESULT = SQLINTEGER;

// Game
#include "../Common/Singleton.h"
#include "Include/DBResult.h"
#include "Include/Connection.h"
