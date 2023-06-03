#pragma once
#include <d2d1.h>

class INT2
{
public:
	INT2();
	INT2(int x, int y);

	operator D2D1_SIZE_U() const;

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
	FLOAT2();
	FLOAT2(FLOAT x, FLOAT y);

	operator D2D1_SIZE_F() const;

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

	void Offset(FLOAT x, FLOAT y);
	BOOL IsContain(FLOAT2 point);
};

using MATRIX = D2D1::Matrix3x2F;

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result{ hr } { }

		const char* what() const override
		{
			static char s_str[64]{};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<UINT>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
			throw com_exception{ hr };
	}
}