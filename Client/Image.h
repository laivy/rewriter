#pragma once
#include "UI.h"

class Image : public UI
{
public:
	Image(const ComPtr<ID2D1Bitmap>& bitmap);

	ID2D1Bitmap* const Get() const;

private:
	ComPtr<ID2D1Bitmap> m_bitmap;
};