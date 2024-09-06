#pragma once
#include "Font.h"

struct ID2D1DeviceContext2;

namespace Graphics::D2D
{
	using Color = D2D1::ColorF;
	using Matrix = D2D1::Matrix3x2F;

	DLL_API bool Begin();
	DLL_API bool End();

	DLL_API ComPtr<ID2D1DeviceContext2> GetContext();

	DLL_API void SetTransform(const Matrix& transform);
	DLL_API void PushClipRect(const RECTF& rect);
	DLL_API void PopClipRect();

	DLL_API void DrawRect(const RECTF& rect, const Color& color);
	DLL_API void DrawText(std::wstring_view text, const INT2& position, const Font& font, const Color& color);
}
