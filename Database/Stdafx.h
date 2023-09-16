#pragma once

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

// Game
#include "../Common/Singleton.h"
#include "Include/Connection.h"

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif