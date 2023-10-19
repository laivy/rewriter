#pragma once
#include <windows.h>

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
#ifdef _DEBUG
		if (FAILED(hr))
			throw;
#endif
	}
}