#pragma once

#if defined _CLIENT || defined _TOOL
struct ID2D1DeviceContext2;
#endif

namespace Resource
{
	class Property;
}

namespace Resource
{
	extern std::map<std::wstring, std::shared_ptr<Property>> g_resources;
#if defined _CLIENT || defined _TOOL
	extern ComPtr<ID2D1DeviceContext2> g_d2dContext;
#endif
}
