#include "Stdafx.h"
#include "NytImage.h"

NytImage::NytImage(const ComPtr<ID2D1Bitmap>& bitmap) : m_bitmap{ bitmap }
{
	m_size.x = bitmap->GetSize().width;
	m_size.y = bitmap->GetSize().height;
}

void NytImage::Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const
{
	MATRIX view{};
	renderTarget->GetTransform(&view);

	MATRIX world{ GetMatrix() };
	renderTarget->SetTransform(world * view);
	renderTarget->DrawBitmap(m_bitmap.Get(), RECTF{ 0.0f, 0.0f, m_size.x, m_size.y });
	renderTarget->SetTransform(view);
}