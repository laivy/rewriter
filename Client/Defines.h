#pragma once
#include <d2d1.h>

using FLOAT2 = D2D1_POINT_2F;
using RECTF = D2D1_RECT_F;

inline FLOAT2 operator+(const FLOAT2& lhs, const FLOAT2& rhs)
{
	FLOAT2 value{ lhs };
	value.x += rhs.x;
	value.y += rhs.y;
	return value;
}

inline FLOAT2 operator-(const FLOAT2& lhs, const FLOAT2& rhs)
{
	FLOAT2 value{ lhs };
	value.x -= rhs.x;
	value.y -= rhs.y;
	return value;
}

inline void operator+=(FLOAT2& lhs, const FLOAT2& rhs)
{
	lhs = lhs + rhs;
}

inline void operator-=(FLOAT2& lhs, const FLOAT2& rhs)
{
	lhs = lhs - rhs;
}

//class FLOAT2 : public D2D1_POINT_2F
//{
//	friend FLOAT2 operator+(const FLOAT2& lhs, const FLOAT2& rhs)
//	{
//
//	}
//};
//
//class RECTF : public D2D1_RECT_F
//{
//
//};