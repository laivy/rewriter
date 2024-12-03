#pragma once

interface ID2D1DeviceContext2;
interface ID2D1BitmapRenderTarget;

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

		ID2D1BitmapRenderTarget* GetRenderTarget() const;

	private:
		ComPtr<ID2D1BitmapRenderTarget> m_renderTarget;
	};

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
		unsigned int rgb{};
		float a{ 1.0f };
	};

	struct Font
	{
		std::wstring name;
		float size{};
	};

	DLL_API void Begin();
	DLL_API bool End();

	DLL_API std::shared_ptr<Resource::Sprite> LoadSprite(std::span<std::byte> binary);
	DLL_API std::shared_ptr<Layer> CreateLayer(const Float2& size);

	DLL_API void SetTransform(const Transform& transform);
	DLL_API void PushClipRect(const RectF& rect);
	DLL_API void PopClipRect();
	DLL_API void PushLayer(const std::shared_ptr<Layer>& layer);
	DLL_API void PopLayer();

	DLL_API void DrawRect(const RectF& rect, const Color& color);
	DLL_API void DrawRoundRect(const RectF& rect, const Float2& radius, const Color& color);
	DLL_API void DrawText(std::wstring_view text, const Font& font, const Color& color, const Float2& position, Pivot pivot = Pivot::LeftTop);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const Float2& position, float opacity = 1.0f);
	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const RectF& rect, float opacity = 1.0f);
	DLL_API void DrawLayer(const std::shared_ptr<Layer>& layer);

	DLL_API TextMetrics GetTextMetrics(std::wstring_view text, const Font& font);
}
