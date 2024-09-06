#pragma once
#ifdef DLL_API
#undef DLL_API
#endif // DLL_API
#define DLL_API __declspec(dllimport)

#if defined _CLIENT || defined _TOOL
struct ID2D1DeviceContext2;
#endif // _CLIENT || _TOOL

#include "../PNG.h"
#include "../Property.h"

namespace Resource
{
#if defined _CLIENT || defined _TOOL
	DLL_API void Initialize(const ComPtr<ID2D1DeviceContext2>& d2dContext);
#endif // _CLIENT || _TOOL
	DLL_API std::shared_ptr<Property> Get(std::wstring_view path);
	DLL_API void Unload(std::wstring_view path);
}
