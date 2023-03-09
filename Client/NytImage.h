#pragma once
#include "ConstantBuffer.h"

class NytImage
{
public:
	NytImage(ID3D12Resource* resource);
	~NytImage() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex = RootParamIndex::TEXTURE0);

	ID3D12Resource* GetResource() const;

private:
	ComPtr<ID3D12Resource> m_resource;
	struct cbImage
	{
		UINT width;
		UINT height;
		FLOAT2 dummy;
	};
	ConstantBuffer<cbImage> m_cbImage;
};