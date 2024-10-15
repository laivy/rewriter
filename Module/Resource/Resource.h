#pragma once

#if defined _CLIENT || defined _TOOL
struct ID2D1DeviceContext2;
#endif

namespace Resource
{
	class Property;

#if defined _CLIENT || defined _TOOL
	extern ComPtr<ID2D1DeviceContext2> g_d2dContext;
	DLL_API void Initialize(const ComPtr<ID2D1DeviceContext2>& d2dContext);
#endif

#ifdef _TOOL
	DLL_API bool Save(const std::shared_ptr<Property>& prop, std::wstring_view path);
#endif

	DLL_API std::shared_ptr<Property> Get(std::wstring_view path);
	DLL_API void Unload(std::wstring_view path);
}
