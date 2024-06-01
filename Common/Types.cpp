#include "Types.h"

INT2::INT2(int32_t x, int32_t y) :
	x{ x },
	y{ y }
{
}

INT2::INT2(const FLOAT2& float2) : 
	INT2{ static_cast<int32_t>(float2.x), static_cast<int32_t>(float2.y) }
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

FLOAT2::FLOAT2(const INT2& int2) :
	FLOAT2{ static_cast<float>(int2.x), static_cast<float>(int2.y) }
{
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

RECTI::RECTI(int32_t left, int32_t top, int32_t right, int32_t bottom) :
	left{ left }, top{ top }, right{ right }, bottom{ bottom }
{

}

RECTI& RECTI::Offset(const INT2& offset)
{
	left += offset.x;
	top += offset.y;
	right += offset.x;
	bottom += offset.y;
	return *this;
}

bool RECTI::IsContain(const INT2& point) const
{
	if (left <= point.x && point.x <= right &&
		top <= point.y && point.y <= bottom)
		return true;
	return false;
}

RECTF::RECTF(float left, float top, float right, float bottom)
{
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
}

RECTF::RECTF(const RECTI& rect)
{
	left = static_cast<float>(rect.left);
	top = static_cast<float>(rect.top);
	right = static_cast<float>(rect.right);
	bottom = static_cast<float>(rect.bottom);
}

RECTF& RECTF::Offset(const FLOAT2& offset)
{
	left += offset.x;
	top += offset.y;
	right += offset.x;
	bottom += offset.y;
	return *this;
}

bool RECTF::IsContain(const FLOAT2& point) const
{
	if (left <= point.x && point.x <= right &&
		top <= point.y && point.y <= bottom)
		return true;
	return false;
}