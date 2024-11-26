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
		DLL_API void Draw(const Float2& position = {});
		DLL_API void Clear();

		ComPtr<ID2D1BitmapRenderTarget> GetTarget() const;

	private:
		ComPtr<ID2D1BitmapRenderTarget> m_target;
	};

	struct TextMetrics
	{
		float left;
		float top;
		float width;
		float height;
	};

	using Color = D2D1::ColorF;
	using Matrix = D2D1::Matrix3x2F;

	DLL_API void Begin();
	DLL_API bool End();

	DLL_API ComPtr<ID2D1DeviceContext2> GetContext();
	DLL_API std::shared_ptr<Layer> CreateLayer(const Float2& size);

	DLL_API void SetTransform(const Matrix& transform);
	DLL_API void PushClipRect(const RectF& rect);
	DLL_API void PopClipRect();

	DLL_API void DrawRect(const RectF& rect, const Color& color);
	DLL_API void DrawRoundRect(const RectF& rect, const Float2& radius, const Color& color);
	DLL_API void DrawText(std::wstring_view text, const Font& font, const Color& color, const Float2& position, Pivot pivot = Pivot::LeftTop);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const Float2& position, float opacity = 1.0f);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const RectF& rect, float opacity = 1.0f);

	DLL_API TextMetrics GetTextMetrics(std::wstring_view text, const Font& font);
}
