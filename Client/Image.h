#pragma once
#include "ConstantBuffer.h"

class Image
{
private:
	struct cbImage
	{
		UINT width;
		UINT height;
		FLOAT2 dummy;
	};

public:
	Image(ID2D1Bitmap* bitmap);
	Image(ID3D12Resource* resource);
	~Image() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const;
	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex = RootParamIndex::TEXTURE0);

private:
	// D2D
	ComPtr<ID2D1Bitmap> m_bitmap;
	FLOAT2 m_size;

	// D3D
	ComPtr<ID3D12Resource> m_resource;
	ConstantBuffer<cbImage> m_cbTexture;
};