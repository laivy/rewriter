#pragma once
#include <d2d1.h>
#include "defines.h"

namespace Util
{
	inline BOOL IsContain(const RECTF& rect, const POINT& point)
	{
		if (rect.left <= point.x && point.x <= rect.right &&
			rect.top <= point.y && point.y <= rect.bottom)
			return TRUE;
		return FALSE;
	}
}