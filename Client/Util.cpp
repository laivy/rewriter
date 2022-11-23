#include "Stdafx.h"
#include "Util.h"

FLOAT2::FLOAT2()
{
	x = 0.0f;
	y = 0.0f;
}

FLOAT2::FLOAT2(FLOAT x, FLOAT y)
{
	this->x = x;
	this->y = y;
}

FLOAT2 operator+(const FLOAT2& lhs, const FLOAT2& rhs)
{
	FLOAT2 value{ lhs };
	value.x += rhs.x;
	value.y += rhs.y;
	return value;
}

FLOAT2 operator-(const FLOAT2& lhs, const FLOAT2& rhs)
{
	FLOAT2 value{ lhs };
	value.x -= rhs.x;
	value.y -= rhs.y;
	return value;
}

void operator+=(FLOAT2& lhs, const FLOAT2& rhs)
{
	lhs = lhs + rhs;
}

void operator-=(FLOAT2& lhs, const FLOAT2& rhs)
{
	lhs = lhs - rhs;
}

namespace Util
{
	BOOL IsContain(const RECTF& rect, const POINT& point)
	{
		if (rect.left <= point.x && point.x <= rect.right &&
			rect.top <= point.y && point.y <= rect.bottom)
			return TRUE;
		return FALSE;
	}
}

RECTF::RECTF()
{
	left = 0.0f;
	top = 0.0f;
	right = 0.0f;
	bottom = 0.0f;
}

RECTF::RECTF(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}