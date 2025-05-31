#pragma once
#include <cstdint>

class Float2;

enum class Pivot
{
	LeftTop,
	CenterTop,
	RightTop,
	LeftCenter,
	Center,
	RightCenter,
	LeftBot,
	CenterBot,
	RightBot
};

class Int2
{
public:
	Int2(int32_t x = 0, int32_t y = 0);
	Int2(float x, float y);
	Int2(const Float2& float2);

	friend Int2 operator+(const Int2& lhs, const Int2& rhs);
	friend Int2 operator-(const Int2& lhs, const Int2& rhs);
	friend void operator+=(Int2& lhs, const Int2& rhs);
	friend void operator-=(Int2& lhs, const Int2& rhs);

public:
	int32_t x;
	int32_t y;
};

class Float2
{
public:
	Float2(float x = 0.0f, float y = 0.0f);
	Float2(int32_t x, int32_t y);
	Float2(const Int2& int2);

	friend Float2 operator+(const Float2& lhs, const Float2& rhs);
	friend Float2 operator-(const Float2& lhs);
	friend Float2 operator-(const Float2& lhs, const Float2& rhs);
	friend void operator+=(Float2& lhs, const Float2& rhs);
	friend void operator-=(Float2& lhs, const Float2& rhs);
	friend Float2 operator*(const Float2& lhs, float rhs);
	friend Float2 operator/(const Float2& lhs, float rhs);

public:
	float x;
	float y;
};

class Float3
{
public:
	Float3(float x = 0.0f, float y = 0.0f, float z = 0.0f);

	Float3 operator+(const Float3& rhs);
	Float3 operator-();
	Float3 operator-(const Float3& rhs);
	Float3& operator+=(const Float3& rhs);
	Float3& operator-=(const Float3& rhs);
	Float3 operator*(float rhs);
	Float3 operator/(float rhs);

public:
	float x;
	float y;
	float z;
};

class Rect
{
public:
	Rect(int32_t left = 0, int32_t top = 0, int32_t right = 0, int32_t bottom = 0);

	Rect& Offset(const Int2& offset);
	bool Contains(const Int2& point) const;

public:
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
};

class RectF
{
public:
	RectF(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f);
	RectF(const Rect& rect);

	RectF& Offset(const Float2& offset);
	bool Contains(const Float2& point) const;

public:
	float left;
	float top;
	float right;
	float bottom;
};

class Matrix
{
public:
	Matrix();

public:
	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		std::array<std::array<float, 4>, 4> m;
	};
};
