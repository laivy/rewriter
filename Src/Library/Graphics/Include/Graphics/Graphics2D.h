#pragma once

#ifdef _DIRECT2D
namespace Resource
{
	struct Sprite;
}

namespace Graphics::D2D
{
	class Layer;

	struct Scale
	{
		Float2 scale{ 1.0f, 1.0f };
		Float2 center{ 0.0f, 0.0f };
	};

	struct Rotation
	{
		float angle{ 0.0f };
		Float2 center{ 0.0f, 0.0f };
	};

	struct Transform
	{
		Scale scale{};
		Rotation rotation{};
		Float2 translation{};
	};

	struct TextMetrics
	{
		float left{};
		float top{};
		float width{};
		float height{};
	};

	struct Color
	{
		GRAPHICS_API Color();
		GRAPHICS_API Color(std::uint32_t argb);

		std::uint32_t rgb;
		float a;
	};

	struct Font
	{
		std::wstring name;
		float size{};
	};

	bool Initialize();
	void Uninitialize();

	GRAPHICS_API void Begin();
	GRAPHICS_API void End();

	GRAPHICS_API Resource::Sprite LoadSprite(std::span<std::byte> binary);
	GRAPHICS_API std::shared_ptr<Layer> CreateLayer(const Float2& size);

	GRAPHICS_API void SetTransform(const Transform& transform);
	GRAPHICS_API void PushClipRect(const RectF& rect);
	GRAPHICS_API void PopClipRect();
	GRAPHICS_API void PushLayer(const std::shared_ptr<Layer>& layer);
	GRAPHICS_API void PopLayer();

	GRAPHICS_API void DrawRect(const RectF& rect, const Color& color);
	GRAPHICS_API void DrawRoundRect(const RectF& rect, const Float2& radius, const Color& color);
	GRAPHICS_API void DrawText(std::wstring_view text, const Font& font, const Color& color, const Float2& position, Pivot pivot = Pivot::LeftTop);
	GRAPHICS_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const Float2& position, float opacity = 1.0f);
	GRAPHICS_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const RectF& rect, float opacity = 1.0f);
	GRAPHICS_API void DrawLayer(const std::shared_ptr<Layer>& layer);

	GRAPHICS_API TextMetrics GetTextMetrics(std::wstring_view text, const Font& font);
}
#endif
