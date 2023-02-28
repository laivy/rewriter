#include "Stdafx.h"
#include "NytImage.h"

NytImage::NytImage(const ComPtr<ID2D1Bitmap>& bitmap) : m_bitmap{ bitmap }
{
	m_size.x = bitmap->GetSize().width;
	m_size.y = bitmap->GetSize().height;
}

NytImage::NytImage(const ComPtr<ID3D12Resource>& resource) : m_resource{ resource }
{
	auto desc{ resource->GetDesc() };
	m_size.x = desc.Width;
	m_size.y = desc.Height;
}

void NytImage::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	d2dContext->DrawBitmap(m_bitmap.Get(), RECTF{ 0.0f, 0.0f, m_size.x, m_size.y });
}