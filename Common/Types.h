#pragma once
#include <cstdint>
#include <d2d1.h>

class INT2
{
public:
	INT2(int32_t x = 0, int32_t y = 0);

private:
	friend INT2 operator+(const INT2& lhs, const INT2& rhs);
	friend INT2 operator-(const INT2& lhs, const INT2& rhs);
	friend void operator+=(INT2& lhs, const INT2& rhs);
	friend void operator-=(INT2& lhs, const INT2& rhs);

public:
	int32_t x;
	int32_t y;
};

class FLOAT2 : public D2D1_POINT_2F
{
public:
	FLOAT2(FLOAT x = 0.0f, FLOAT y = 0.0f);

private:
	friend FLOAT2 operator+(const FLOAT2& lhs, const FLOAT2& rhs);
	friend FLOAT2 operator-(const FLOAT2& lhs, const FLOAT2& rhs);
	friend void operator+=(FLOAT2& lhs, const FLOAT2& rhs);
	friend void operator-=(FLOAT2& lhs, const FLOAT2& rhs);
	friend FLOAT2 operator*(const FLOAT2& lhs, FLOAT rhs);
	friend FLOAT2 operator/(const FLOAT2& lhs, FLOAT rhs);
};

class RECTI
{
public:
	RECTI();
	RECTI(int left, int top, int right, int bottom);

	RECTI& Offset(int x, int y);
	bool IsContain(const INT2& point);

public:
	int left;
	int top;
	int right;
	int bottom;
};

class RECTF : public D2D1_RECT_F
{
public:
	RECTF();
	RECTF(FLOAT left, FLOAT top, FLOAT right, FLOAT bottom);

	RECTF& Offset(FLOAT x, FLOAT y);
	bool IsContain(FLOAT2 point);
};

using MATRIX = D2D1::Matrix3x2F;

using ButtonID = unsigned int;
using CharacterID = unsigned int;
using GameObjectID = unsigned int;