#include "Stdafx.h"
#include "Globals.h"
#include "Graphics2D.h"

namespace Graphics::D2D
{
	struct ColorCompare
	{
		bool operator()(const Color& lhs, const Color& rhs) const
		{
			if (lhs.r != rhs.r)
				return lhs.r < rhs.r;
			if (lhs.g != rhs.g)
				return lhs.g < rhs.g;
			if (lhs.b != rhs.b)
				return lhs.b < rhs.b;
			return lhs.a < rhs.a;
		}
	};

	std::map<Font, ComPtr<IDWriteTextFormat>> textFormats;
	std::map<Color, ComPtr<ID2D1SolidColorBrush>, ColorCompare> colorBrushes;

	DLL_API bool Begin()
	{
		d3d11On12Device->AcquireWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);
		d2dContext->SetTarget(d2dRenderTargets[frameIndex].Get());
		d2dContext->BeginDraw();
		return true;
	}

	DLL_API bool End()
	{
		if (FAILED(d2dContext->EndDraw()))
			return false;
		d3d11On12Device->ReleaseWrappedResources(wrappedBackBuffers[frameIndex].GetAddressOf(), 1);
		d3d11DeviceContext->Flush();
		return true;
	}

	DLL_API void D2D::SetTransform(const Matrix& transform)
	{
		d2dContext->SetTransform(transform);
	}

	DLL_API void D2D::PushClipRect(const RECTF& rect)
	{
		d2dContext->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);
	}

	DLL_API void D2D::PopClipRect()
	{
		d2dContext->PopAxisAlignedClip();
	}

	DLL_API void DrawRect(const RECTF& rect, const Color& color)
	{
		ComPtr<ID2D1SolidColorBrush> brush{};
		d2dContext->CreateSolidColorBrush(color, &brush);
		d2dContext->FillRectangle(rect, brush.Get());
	}

	DLL_API void DrawText(std::wstring_view text, const INT2& position, const Font& font, const Color& color)
	{
		ComPtr<IDWriteTextFormat> textFormat;
		if (textFormats.contains(font))
		{
			textFormat = textFormats[font];
		}
		else
		{
			dwriteFactory->CreateTextFormat(font.fontName.data(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, font.fontSize, L"", &textFormat);
			switch (font.vAlign)
			{
			case Font::VAlign::Top:
				textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
				break;
			case Font::VAlign::Center:
				textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
				break;
			case Font::VAlign::Bottom:
				textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
				break;
			}
			switch (font.hAlign)
			{
			case Font::HAlign::Left:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
				break;
			case Font::HAlign::Center:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				break;
			case Font::HAlign::Right:
				textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
				break;
			}
			textFormats.emplace(font, textFormat);
		}

		ComPtr<ID2D1SolidColorBrush> colorBrush;
		if (colorBrushes.contains(color))
		{
			colorBrush = colorBrushes[color];
		}
		else
		{
			d2dContext->CreateSolidColorBrush(color, &colorBrush);
			colorBrushes.emplace(color, colorBrush);
		}

		ComPtr<IDWriteTextLayout> textLayout;
		dwriteFactory->CreateTextLayout(text.data(), static_cast<UINT32>(text.size()), textFormat.Get(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), &textLayout);
		d2dContext->DrawTextLayout(FLOAT2{ position }, textLayout.Get(), colorBrush.Get());
	}
}
