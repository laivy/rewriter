#include "Stdafx.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace Renderer2D
{
	void Begin()
	{
		Renderer::d3d11On12Device->AcquireWrappedResources(Renderer::wrappedBackBuffers[Renderer::frameIndex].GetAddressOf(), 1);
		Renderer::ctx->SetTarget(Renderer::d2dRenderTargets[Renderer::frameIndex].Get());
		Renderer::ctx->BeginDraw();
	}

	void End()
	{
		DX::ThrowIfFailed(Renderer::ctx->EndDraw());
		Renderer::d3d11On12Device->ReleaseWrappedResources(Renderer::wrappedBackBuffers[Renderer::frameIndex].GetAddressOf(), 1);
		Renderer::d3d11DeviceContext->Flush();
	}

	void DrawRect(const RECTF& rect, D2D1::ColorF color)
	{
		ComPtr<ID2D1SolidColorBrush> brush{};
		Renderer::ctx->CreateSolidColorBrush(color, &brush);
		Renderer::ctx->FillRectangle(rect, brush.Get());
	}

	void DrawRoundRect(const RECTF& rect, const FLOAT2& radius, D2D1::ColorF color)
	{
		D2D1_ROUNDED_RECT roundRect{};
		roundRect.rect = rect;
		roundRect.radiusX = radius.x;
		roundRect.radiusY = radius.y;

		ComPtr<ID2D1SolidColorBrush> brush{};
		Renderer::ctx->CreateSolidColorBrush(color, &brush);
		Renderer::ctx->FillRoundedRectangle(roundRect, brush.Get());
	}

	void DrawImage(const std::shared_ptr<Resource::PNG>& image, const FLOAT2& position, float opacity)
	{
		INT2 size{ image->GetSize() };
		RECTF rect{ 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y) };
		rect.Offset(position);
		DrawImage(image, rect, opacity);
	}

	void DrawImage(const std::shared_ptr<Resource::PNG>& image, const RECTF& rect, float opacity)
	{
		Renderer::ctx->DrawBitmap(static_cast<ID2D1Bitmap*>(image->Get()), rect, opacity);
	}

	ComPtr<IDWriteTextFormat> CreateTextFormat(std::wstring_view fontName, int fontSize, DWRITE_TEXT_ALIGNMENT textAlignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment)
	{
		ComPtr<IDWriteTextFormat> textFormat;
		Renderer::dwriteFactory->CreateTextFormat(
			fontName.data(),
			nullptr,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			static_cast<float>(fontSize),
			L"",
			&textFormat
		);
		textFormat->SetTextAlignment(textAlignment);
		textFormat->SetParagraphAlignment(paragraphAlignment);
		return textFormat;
	}

	ComPtr<IDWriteTextLayout> CreateTextLayout(std::wstring_view text, const ComPtr<IDWriteTextFormat>& textFormat, int maxWidth, int maxHeight)
	{
		ComPtr<IDWriteTextLayout> textLayout;
		Renderer::dwriteFactory->CreateTextLayout(
			text.data(),
			static_cast<UINT32>(text.size()),
			textFormat.Get(),
			static_cast<float>(maxWidth),
			static_cast<float>(maxHeight),
			&textLayout
		);
		return textLayout;
	}

	void DrawText(const INT2& position, const ComPtr<IDWriteTextLayout>& textLayout, D2D1::ColorF color)
	{
		ComPtr<ID2D1SolidColorBrush> brush;
		Renderer::ctx->CreateSolidColorBrush(color, &brush);
		Renderer::ctx->DrawTextLayout(
			FLOAT2{ position },
			textLayout.Get(),
			brush.Get()
		);
	}

	void DrawText(const INT2& position, std::wstring_view text, std::wstring_view fontName, int fontSize, D2D1::ColorF color, DWRITE_TEXT_ALIGNMENT textAlignment, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment)
	{
		ComPtr<ID2D1SolidColorBrush> brush;
		Renderer::ctx->CreateSolidColorBrush(color, &brush);

		auto size{ Renderer::ctx->GetSize() };
		auto textFormat{ CreateTextFormat(fontName, fontSize, textAlignment, paragraphAlignment) };
		Renderer::ctx->DrawText(
			text.data(),
			static_cast<UINT32>(text.size()),
			textFormat.Get(),
			{ static_cast<float>(position.x), static_cast<float>(position.y), size.width, size.height },
			brush.Get()
		);
	}
}