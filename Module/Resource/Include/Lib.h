#pragma once
#ifdef DLL_API
#undef DLL_API
#endif // DLL_API
#define DLL_API __declspec(dllimport)

#include "../PNG.h"
#include "../Property.h"

#ifdef _CLIENT
struct ID2D1DeviceContext2;
#endif // _CLIENT

namespace Resource
{
#ifdef _CLIENT
	DLL_API void Init(const ComPtr<ID2D1DeviceContext2>& ctx);
#endif // _CLIENT
	DLL_API std::shared_ptr<Property> Get(std::wstring_view path);
	DLL_API void Unload(std::wstring_view path);
}
