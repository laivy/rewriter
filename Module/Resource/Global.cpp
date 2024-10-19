#include "Stdafx.h"
#include "Global.h"
#include "Property.h"

namespace Resource
{
	std::map<std::wstring, std::shared_ptr<Property>> g_resources;
#if defined _CLIENT || defined _TOOL
	ComPtr<ID2D1DeviceContext2> g_d2dContext;
#endif
}
