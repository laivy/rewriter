#pragma once
#include "ConstantBuffer.h"

class NytImage
{
public:
	NytImage(ID2D1Bitmap* bitmap);
	NytImage(ID3D12Resource* resource);
	~NytImage() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const;
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex = RootParamIndex::TEXTURE0);

private:
	// D2D
	ComPtr<ID2D1Bitmap> m_bitmap;
	FLOAT2 m_size;

	// D3D
	ComPtr<ID3D12Resource> m_resource;
	struct cbTexture
	{
		UINT width;
		UINT height;
		FLOAT2 dummy;
	};
	ConstantBuffer<cbTexture> m_cbTexture;
};