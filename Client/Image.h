#pragma once
#include "ConstantBuffer.h"

class Image
{
private:
	struct D2DImage
	{
		ComPtr<ID2D1Bitmap> bitmap;
		FLOAT2 size;
	};

	struct D3DImage
	{
		struct cbImage
		{
			UINT width;
			UINT height;
			FLOAT2 dummy;
		};

		ComPtr<ID3D12Resource> resource;
		ConstantBuffer<cbImage> cbImage;
	};

public:
	Image();
	Image(ID2D1Bitmap* bitmap);
	Image(ID3D12Resource* resource);
	~Image() = default;

	void Render(const ComPtr<ID2D1DeviceContext2>& d2dContext, FLOAT x, FLOAT y) const;
	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex = RootParamIndex::TEXTURE0);

private:
	std::variant<D2DImage, D3DImage> m_data;
};