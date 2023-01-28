#include "Stdafx.h"
#include "NytImage.h"

NytImage::NytImage(const ComPtr<ID2D1Bitmap>& bitmap) : m_bitmap{ bitmap }
{

}

void NytImage::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	auto size{ m_bitmap->GetSize() };
	renderTarget->DrawBitmap(m_bitmap.Get(), RECTF{ 0.0f, 0.0f, size.width, size.height });
}