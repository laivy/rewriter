#pragma once

namespace Renderer2D
{
	void Begin();
	void End();

	void DrawRect(const RECTF& rect, D2D1::ColorF color = D2D1::ColorF::Black);
	void DrawRoundRect(const RECTF& rect, const FLOAT2& radius, D2D1::ColorF color);
	void DrawImage(const std::shared_ptr<Resource::PNG>& image, const FLOAT2& position, float opacity = 1.0f);
	void DrawImage(const std::shared_ptr<Resource::PNG>& image, const RECTF& rect, float opacity = 1.0f);
	
	ComPtr<IDWriteTextFormat> CreateTextFormat(std::wstring_view fontName, int fontSize, DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	ComPtr<IDWriteTextLayout> CreateTextLayout(std::wstring_view text, const ComPtr<IDWriteTextFormat>& textFormat, int maxWidth, int maxHeight);
	void DrawText(const INT2& position, const ComPtr<IDWriteTextLayout>& textLayout, D2D1::ColorF color);
	void DrawText(const INT2& position, std::wstring_view text, std::wstring_view fontName, int fontSize, D2D1::ColorF color, DWRITE_TEXT_ALIGNMENT textAlignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}