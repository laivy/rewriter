#include "Stdafx.h"
#include "NytUI.h"

NytUI::NytUI(ID2D1Bitmap* bitmap) : m_bitmap{ bitmap }
{
	m_size.x = bitmap->GetSize().width;
	m_size.y = bitmap->GetSize().height;
}

void NytUI::Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const
{
	d2dContext->DrawBitmap(m_bitmap.Get(), RECTF{ 0.0f, 0.0f, m_size.x, m_size.y });
}