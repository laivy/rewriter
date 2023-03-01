#pragma once
#include "GameObject.h"

class NytUI : public GameObject
{
public:
	NytUI(const ComPtr<ID2D1Bitmap>& bitmap);
	~NytUI() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext) const;

private:
	ComPtr<ID2D1Bitmap> m_bitmap;
};