#pragma once
#include "Common/Stdafx.h"

// C/C++
#include <execution>
#include <format>
#include <semaphore>

// Windows
#include <WinSock2.h> // MSWSock.h 보다 위에 있어야함
#include <MSWSock.h>
#include <WS2tcpip.h>

// Module
#include "Module/Resource/Include/Lib.h"

// Project
#include "Common/Packet.h"
#include "Common/Protocol.h"
#include "Common/Request.h"
