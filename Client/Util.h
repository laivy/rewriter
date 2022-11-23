#pragma once
#include <d2d1.h>

class FLOAT2 : public D2D1_POINT_2F
{
public:
	FLOAT2();
	FLOAT2(FLOAT x, FLOAT y);

private:
	friend FLOAT2 operator+(const FLOAT2& lhs, const FLOAT2& rhs);
	friend FLOAT2 operator-(const FLOAT2& lhs, const FLOAT2& rhs);
	friend void operator+=(FLOAT2& lhs, const FLOAT2& rhs);
	friend void operator-=(FLOAT2& lhs, const FLOAT2& rhs);
};

class RECTF : public D2D1_RECT_F
{
public:
	RECTF();
	RECTF(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom);
};

namespace Util
{
	BOOL IsContain(const RECTF& rect, const POINT& point);
}