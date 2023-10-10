#include "Types.h"

INT2::INT2(int x, int y) :
	x{ x },
	y{ y }
{

}

INT2 operator+(const INT2& lhs, const INT2& rhs)
{
	INT2 value{ lhs };
	value.x += rhs.x;
	value.y += rhs.y;
	return value;
}

INT2 operator-(const INT2& lhs, const INT2& rhs)
{
	INT2 value{ lhs };
	value.x -= rhs.x;
	value.y -= rhs.y;
	return value;
}

void operator+=(INT2& lhs, const INT2& rhs)
{
	lhs = lhs + rhs;
}

void operator-=(INT2& lhs, const INT2& rhs)
{
	lhs = lhs - rhs;
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

FLOAT2 operator*(const FLOAT2& lhs, FLOAT rhs)
{
	return { lhs.x * rhs, lhs.y * rhs };
}

FLOAT2 operator/(const FLOAT2& lhs, FLOAT rhs)
{
	return { lhs.x / rhs, lhs.y / rhs };
}