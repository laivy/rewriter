#include "Stdafx.h"
#include "Global.h"
#include "Graphics2D.h"

namespace
{
	struct FontCompare
	{
		bool operator()(const Graphics::D2D::Font& lhs, const Graphics::D2D::Font& rhs) const
		{
			if (lhs.name != rhs.name)
				return lhs.name < rhs.name;
			return lhs.size < rhs.size;
		}
	};

	struct ColorCompare
	{
		bool operator()(const Graphics::D2D::Color& lhs, const Graphics::D2D::Color& rhs) const
		{
			if (lhs.rgb != rhs.rgb)
				return lhs.rgb < rhs.rgb;
			return lhs.a < rhs.a;
		}
	};

	std::map<Graphics::D2D::Font, ComPtr<IDWriteTextFormat>, FontCompare> g_textFormats;
	std::map<Graphics::D2D::Color, ComPtr<ID2D1SolidColorBrush>, ColorCompare> g_colorBrushes;

	ComPtr<IDWriteTextFormat> GetTextFormat(const Graphics::D2D::Font& font)
	{
		ComPtr<IDWriteTextFormat> textFormat;
		if (g_textFormats.contains(font))
		{
			textFormat = g_textFormats[font];
		}
		else
		{
			Graphics::g_dwriteFactory->CreateTextFormat(font.name.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, font.size, L"", &textFormat);
			textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			g_textFormats.emplace(font, textFormat);
		}
		return textFormat;
	}

	ComPtr<ID2D1SolidColorBrush> GetColorBrush(const Graphics::D2D::Color& color)
	{
		ComPtr<ID2D1SolidColorBrush> colorBrush;
		if (g_colorBrushes.contains(color))
		{
			colorBrush = g_colorBrushes[color];
		}
		else
		{
			Graphics::g_d2dContext->CreateSolidColorBrush(D2D1::ColorF{ color.rgb, color.a }, &colorBrush);
			g_colorBrushes.emplace(color, colorBrush);
		}
		return colorBrush;
	}
}

namespace Graphics::D2D
{
	Layer::Layer(ComPtr<ID2D1BitmapRenderTarget> target) :
		m_renderTarget{ target }
	{
	}

	ID2D1BitmapRenderTarget* Layer::GetRenderTarget() const
	{
		return m_renderTarget.Get();
	}

	DLL_API Color::Color() :
		rgb{ 0 },
		a{ 1.0f }
	{
	}

	DLL_API Color::Color(uint32_t argb)
	{
		rgb = 0x00FFFFFF & argb;
		a = (0xFF000000 & argb) / static_cast<float>(0xFF);
	}

	DLL_API void Begin()
	{
		g_d3d11On12Device->AcquireWrappedResources(g_wrappedBackBuffers[g_frameIndex].GetAddressOf(), 1);
		g_d2dContext->SetTarget(g_d2dRenderTargets[g_frameIndex].Get());
		g_d2dContext->BeginDraw();
		g_d2dCurrentRenderTargets.clear();
		g_d2dCurrentRenderTargets.push_back(g_d2dContext.Get());
	}

	DLL_API bool End()
	{
		if (FAILED(g_d2dContext->EndDraw()))
			return false;
		g_d3d11On12Device->ReleaseWrappedResources(g_wrappedBackBuffers[g_frameIndex].GetAddressOf(), 1);
		g_d3d11DeviceContext->Flush();
		return true;
	}

	DLL_API std::shared_ptr<Resource::Sprite> LoadSprite(std::span<std::byte> binary)
	{
		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<ID2D1Bitmap> bitmap;

		if (FAILED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
			return nullptr;
		if (FAILED(factory->CreateStream(&stream)))
			return nullptr;
		if (FAILED(stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(binary.data()), static_cast<DWORD>(binary.size()))))
			return nullptr;
		if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder)))
			return nullptr;
		if (FAILED(factory->CreateFormatConverter(&converter)))
			return nullptr;
		if (FAILED(decoder->GetFrame(0, &frameDecode)))
			return nullptr;
		if (FAILED(converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return nullptr;
		if (FAILED(g_d2dContext->CreateBitmapFromWicBitmap(converter.Get(), bitmap.GetAddressOf())))
			return nullptr;

		auto size{ bitmap->GetSize() };
		auto sprite{ std::make_shared<Resource::Sprite>(bitmap, Float2{ size.width, size.height }) };
#ifdef _TOOL
		sprite->SetBinary(binary);
#endif
		return sprite;
	}

	DLL_API std::shared_ptr<Layer> CreateLayer(const Float2& size)
	{
		ComPtr<ID2D1BitmapRenderTarget> target;
		if (FAILED(g_d2dContext->CreateCompatibleRenderTarget(D2D1_SIZE_F{ size.x, size.y }, &target)))
			return nullptr;
		return std::make_shared<Layer>(target);
	}

	DLL_API void SetTransform(const Transform& transform)
	{
		g_d2dCurrentRenderTargets.back()->SetTransform(
			D2D1::Matrix3x2F::Scale(transform.scale.scale.x, transform.scale.scale.y, D2D1::Point2F(transform.scale.center.x, transform.scale.center.y)) *
			D2D1::Matrix3x2F::Rotation(transform.rotation.angle, D2D1::Point2F(transform.rotation.center.x, transform.rotation.center.y)) *
			D2D1::Matrix3x2F::Translation(transform.translation.x, transform.translation.y)
		);
	}

	DLL_API void PushClipRect(const RectF& rect)
	{
		g_d2dCurrentRenderTargets.back()->PushAxisAlignedClip(D2D1_RECT_F{ rect.left, rect.top, rect.right, rect.bottom }, D2D1_ANTIALIAS_MODE_ALIASED);
	}

	DLL_API void PopClipRect()
	{
		g_d2dCurrentRenderTargets.back()->PopAxisAlignedClip();
	}

	DLL_API void PushLayer(const std::shared_ptr<Layer>& layer)
	{
		auto rt{ layer->GetRenderTarget() };
		if (!rt)
			return;

		rt->BeginDraw();
		g_d2dCurrentRenderTargets.push_back(rt);
	}

	DLL_API void PopLayer()
	{
		g_d2dCurrentRenderTargets.back()->EndDraw();
		g_d2dCurrentRenderTargets.pop_back();
	}

	DLL_API void DrawRect(const RectF& rect, const Color& color)
	{
		g_d2dCurrentRenderTargets.back()->FillRectangle(D2D1_RECT_F{ rect.left, rect.top, rect.right, rect.bottom }, GetColorBrush(color).Get());
	}

	DLL_API void DrawRoundRect(const RectF& rect, const Float2& radius, const Color& color)
	{
		D2D1_ROUNDED_RECT roundRect{};
		roundRect.rect = D2D1_RECT_F{ rect.left, rect.top, rect.right, rect.bottom };
		roundRect.radiusX = radius.x;
		roundRect.radiusY = radius.y;
		g_d2dCurrentRenderTargets.back()->FillRoundedRectangle(roundRect, GetColorBrush(color).Get());
	}

	DLL_API void DrawText(std::wstring_view text, const Font& font, const Color& color, const Float2& position, Pivot pivot)
	{
		auto textFormat{ GetTextFormat(font) };
		auto colorBrush{ GetColorBrush(color) };

		ComPtr<IDWriteTextLayout> textLayout;
		g_dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), &textLayout);

		DWRITE_TEXT_METRICS metrics{};
		textLayout->GetMetrics(&metrics);
		Float2 offset{};
		switch (pivot)
		{
		case Pivot::LeftTop:
			break;
		case Pivot::CenterTop:
			offset.x -= metrics.width / 2.0f;
			break;
		case Pivot::RightTop:
			offset.x -= metrics.width;
			break;
		case Pivot::LeftCenter:
			offset.y -= metrics.height / 2.0f;
			break;
		case Pivot::Center:
			offset.x -= metrics.width / 2.0f;
			offset.y -= metrics.height / 2.0f;
			break;
		case Pivot::RightCenter:
			offset.x -= metrics.width;
			offset.y -= metrics.height / 2.0f;
			break;
		case Pivot::LeftBot:
			offset.y -= metrics.height;
			break;
		case Pivot::CenterBot:
			offset.x -= metrics.width / 2.0f;
			offset.y -= metrics.height;
			break;
		case Pivot::RightBot:
			offset.x -= metrics.width;
			offset.y -= metrics.height;
			break;
		default:
			break;
		}
		D2D1_POINT_2F origin{};
		origin.x = position.x + offset.x;
		origin.y = position.y + offset.y;
		g_d2dCurrentRenderTargets.back()->DrawTextLayout(origin, textLayout.Get(), colorBrush.Get());
	}

	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const Float2& position, float opacity)
	{
		auto size{ sprite->GetSize() };
		DrawSprite(sprite, RectF{ 0.0f, 0.0f, size.x, size.y }.Offset(position), opacity);
	}

	DLL_API void DrawSprite(const std::shared_ptr<Resource::Sprite>& sprite, const RectF& rect, float opacity)
	{
		g_d2dCurrentRenderTargets.back()->DrawBitmap(static_cast<ID2D1Bitmap*>(sprite->Get()), D2D1_RECT_F{ rect.left, rect.top, rect.right, rect.bottom }, opacity);
	}

	DLL_API void DrawLayer(const std::shared_ptr<Layer>& layer)
	{
		auto rt{ layer->GetRenderTarget() };
		if (!rt)
			return;

		ID2D1Bitmap* bitmap{};
		if (FAILED(rt->GetBitmap(&bitmap)))
			return;

		g_d2dCurrentRenderTargets.back()->DrawBitmap(bitmap);
	}

	DLL_API TextMetrics GetTextMetrics(std::wstring_view text, const Font& font)
	{
		auto textFormat{ GetTextFormat(font) };

		ComPtr<IDWriteTextLayout> textLayout;
		g_dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), &textLayout);

		Float2 pos{};
		DWRITE_HIT_TEST_METRICS metrics{};
		textLayout->HitTestTextPosition(
			static_cast<UINT32>(text.size()),
			TRUE,
			&pos.x,
			&pos.y,
			&metrics
		);

		TextMetrics textMetrics{};
		textMetrics.left = metrics.left;
		textMetrics.top = metrics.top;
		textMetrics.width = metrics.width;
		textMetrics.height = metrics.height;
		return textMetrics;
	}
}
