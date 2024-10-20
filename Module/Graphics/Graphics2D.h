#pragma once
#include "Font.h"

struct ID2D1DeviceContext2;

namespace Resource
{
	class Sprite;
}

namespace Graphics::D2D
{
	class Layer
	{
	public:
		Layer(ComPtr<ID2D1BitmapRenderTarget> target);
		~Layer() = default;

		DLL_API void Begin();
		DLL_API bool End();
		DLL_API void Draw(const FLOAT2& position = {});
		DLL_API void Clear();

		ComPtr<ID2D1BitmapRenderTarget> GetTarget() const;

	private:
		ComPtr<ID2D1BitmapRenderTarget> m_target;
	};

	using Color = D2D1::ColorF;
	using Matrix = D2D1::Matrix3x2F;

	DLL_API void Begin();
	DLL_API bool End();

	DLL_API ComPtr<ID2D1DeviceContext2> GetContext();
	DLL_API std::shared_ptr<Layer> CreateLayer(const FLOAT2& size);

	DLL_API void SetTransform(const Matrix& transform);
	DLL_API void PushClipRect(const RECTF& rect);
	DLL_API void PopClipRect();

	DLL_API void DrawRect(const RECTF& rect, const Color& color);
	DLL_API void DrawRoundRect(const RECTF& rect, const FLOAT2& radius, const Color& color);
	DLL_API void DrawText(std::wstring_view text, const Font& font, const Color& color, const FLOAT2& position, Pivot pivot);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const FLOAT2& position, float opacity = 1.0f);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const RECTF& rect, float opacity = 1.0f);
}
