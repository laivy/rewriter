#include "Stdafx.h"
#include "Util.h"

INT2::INT2()
{
	x = 0;
	y = 0;
}

INT2::INT2(int x, int y)
{
	this->x = static_cast<LONG>(x);
	this->y = static_cast<LONG>(y);
}

INT2::operator D2D1_SIZE_U() const
{
	return D2D1_SIZE_U{ static_cast<UINT>(x), static_cast<UINT>(y) };
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

FLOAT2::operator D2D1_SIZE_F() const
{
	return D2D1_SIZE_F{ x, y };
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

void RECTF::Offset(FLOAT x, FLOAT y)
{
	left += x;
	top += y;
	right += x;
	bottom += y;
}

BOOL RECTF::IsContain(FLOAT2 point)
{
	if (left <= point.x && point.x <= right &&
		top <= point.y && point.y <= bottom)
		return TRUE;
	return FALSE;
}