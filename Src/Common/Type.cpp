#include "Pch.h"
#include "Type.h"

Int2::Int2(int32_t x, int32_t y) :
	x{ x },
	y{ y }
{
}

Int2::Int2(float x, float y) :
	x{ static_cast<int32_t>(x) },
	y{ static_cast<int32_t>(y) }
{
}

Int2::Int2(const Float2& float2) :
	x{ static_cast<int32_t>(float2.x) },
	y{ static_cast<int32_t>(float2.y) }
{
}

Int2 operator+(const Int2& lhs, const Int2& rhs)
{
	Int2 value{ lhs };
	value.x += rhs.x;
	value.y += rhs.y;
	return value;
}

Int2 operator-(const Int2& lhs, const Int2& rhs)
{
	Int2 value{ lhs };
	value.x -= rhs.x;
	value.y -= rhs.y;
	return value;
}

void operator+=(Int2& lhs, const Int2& rhs)
{
	lhs = lhs + rhs;
}

void operator-=(Int2& lhs, const Int2& rhs)
{
	lhs = lhs - rhs;
}

Float2::Float2(float x, float y) :
	x{ x },
	y{ y }
{
}

Float2::Float2(int32_t x, int32_t y)
{
	this->x = static_cast<float>(x);
	this->y = static_cast<float>(y);
}

Float2::Float2(const Int2& int2) :
	x{ static_cast<float>(int2.x) },
	y{ static_cast<float>(int2.y) }
{
}

Float2 operator+(const Float2& lhs, const Float2& rhs)
{
	Float2 value{ lhs };
	value.x += rhs.x;
	value.y += rhs.y;
	return value;
}

Float2 operator-(const Float2& lhs)
{
	return Float2{ -lhs.x, -lhs.y };
}

Float2 operator-(const Float2& lhs, const Float2& rhs)
{
	Float2 value{ lhs };
	value.x -= rhs.x;
	value.y -= rhs.y;
	return value;
}

void operator+=(Float2& lhs, const Float2& rhs)
{
	lhs = lhs + rhs;
}

void operator-=(Float2& lhs, const Float2& rhs)
{
	lhs = lhs - rhs;
}

Float2 operator*(const Float2& lhs, float rhs)
{
	return Float2{ lhs.x * rhs, lhs.y * rhs };
}

Float2 operator/(const Float2& lhs, float rhs)
{
	return Float2{ lhs.x / rhs, lhs.y / rhs };
}

Float3::Float3(float x, float y, float z) :
	x{ x },
	y{ y },
	z{ z }
{
}

Float3 Float3::operator+(const Float3& rhs)
{
	return Float3{ x + rhs.x, y + rhs.y, z + rhs.z };
}

Float3 Float3::operator-()
{
	return Float3{ -x, -y, -z };
}

Float3 Float3::operator-(const Float3& rhs)
{
	return Float3{ x - rhs.x, y - rhs.y, z - rhs.z };
}

Float3& Float3::operator+=(const Float3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Float3& Float3::operator-=(const Float3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Float3 Float3::operator*(float rhs)
{
	return Float3{ x * rhs, y * rhs, z * rhs };
}

Float3 Float3::operator/(float rhs)
{
	return Float3{ x / rhs, y / rhs, z / rhs };
}

Rect::Rect(int32_t left, int32_t top, int32_t right, int32_t bottom) :
	left{ left },
	top{ top },
	right{ right },
	bottom{ bottom }
{
}

Rect& Rect::Offset(const Int2& offset)
{
	left += offset.x;
	top += offset.y;
	right += offset.x;
	bottom += offset.y;
	return *this;
}

bool Rect::Contains(const Int2& point) const
{
	if (left <= point.x && point.x <= right &&
		top <= point.y && point.y <= bottom)
		return true;
	return false;
}

RectF::RectF(float left, float top, float right, float bottom) :
	left{ left },
	top{ top },
	right{ right },
	bottom{ bottom }
{
}

RectF::RectF(const Rect& rect) :
	left{ static_cast<float>(rect.left) },
	top{ static_cast<float>(rect.top) },
	right{ static_cast<float>(rect.right) },
	bottom{ static_cast<float>(rect.bottom) }
{
}

RectF& RectF::Offset(const Float2& offset)
{
	left += offset.x;
	top += offset.y;
	right += offset.x;
	bottom += offset.y;
	return *this;
}

bool RectF::Contains(const Float2& point) const
{
	if (left <= point.x && point.x <= right &&
		top <= point.y && point.y <= bottom)
		return true;
	return false;
}
