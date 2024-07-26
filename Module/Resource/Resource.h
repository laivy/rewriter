#pragma once

struct ID2D1DeviceContext2;

namespace Resource
{
#if defined _CLIENT || defined _TOOL
	extern ComPtr<ID2D1DeviceContext2> g_ctx;
#endif
}