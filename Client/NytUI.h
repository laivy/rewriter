#pragma once

class NytUI
{
public:
	NytUI(ID2D1Bitmap* bitmap);
	~NytUI() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const;

private:
	ComPtr<ID2D1Bitmap> m_bitmap;
	FLOAT2 m_size;
};