#pragma once
#include "../Property.h"

struct ID2D1DeviceContext2;

namespace Resource
{
#ifdef _CLIENT
	__declspec(dllexport) void Init(const ComPtr<ID2D1DeviceContext2>& ctx);
#endif

	__declspec(dllexport) std::shared_ptr<Property> Get(std::wstring_view path);
	__declspec(dllexport) void Unload(std::wstring_view path);
}