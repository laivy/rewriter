#pragma once
#include "GameObject.h"

class NytImage : public GameObject
{
public:
	NytImage(const ComPtr<ID2D1Bitmap>& bitmap);
	~NytImage() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

private:
	ComPtr<ID2D1Bitmap> m_bitmap;
};