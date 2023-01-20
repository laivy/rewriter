#include "Stdafx.h"
#include "Image.h"

Image::Image(const ComPtr<ID2D1Bitmap>& bitmap) : m_bitmap{ bitmap }
{

}

ID2D1Bitmap* const Image::Get() const
{
	return m_bitmap.Get();
}
