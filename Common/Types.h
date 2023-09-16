#pragma once
#include <d2d1.h>

class INT2
{
public:
	INT2(int x = 0, int y = 0);

private:
	friend INT2 operator+(const INT2& lhs, const INT2& rhs);
	friend INT2 operator-(const INT2& lhs, const INT2& rhs);
	friend void operator+=(INT2& lhs, const INT2& rhs);
	friend void operator-=(INT2& lhs, const INT2& rhs);

public:
	int x;
	int y;
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