#pragma once
#include <cstdint>
#include <d2d1.h>

class FLOAT2;

class INT2
{
public:
	INT2(int32_t x = 0, int32_t y = 0);
	INT2(float x, float y);
	INT2(const FLOAT2& float2);

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
	FLOAT2(int32_t x, int32_t y);
	FLOAT2(const INT2& int2);

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
	RECTI(int32_t left = 0, int32_t top = 0, int32_t right = 0, int32_t bottom = 0);

	RECTI& Offset(const INT2& offset);
	bool Contains(const INT2& point) const;

public:
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
};

class RECTF : public D2D1_RECT_F
{
public:
	RECTF(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f);
	RECTF(const RECTI& rect);

	RECTF& Offset(const FLOAT2& offset);
	bool Contains(const FLOAT2& point) const;
};

using MATRIX = D2D1::Matrix3x2F;

using ButtonID = unsigned int;
using CharacterID = unsigned int;
using GameObjectID = unsigned int;
