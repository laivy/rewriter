#pragma once
#include "GameObject.h"

class NytImage : public GameObject
{
public:
	NytImage(const ComPtr<ID2D1Bitmap>& bitmap);
	NytImage(const ComPtr<ID3D12Resource>& resource);
	~NytImage() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& renderTarget) const;

private:
	ComPtr<ID2D1Bitmap> m_bitmap;
	ComPtr<ID3D12Resource> m_resource;
};