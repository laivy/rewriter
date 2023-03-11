#pragma once
#include "ConstantBuffer.h"

class NytImage
{
public:
	NytImage(ID3D12Resource* resource);
	~NytImage() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex = RootParamIndex::TEXTURE0);

private:
	ComPtr<ID3D12Resource> m_resource;
	struct cbTexture
	{
		UINT width;
		UINT height;
		FLOAT2 dummy;
	};
	ConstantBuffer<cbTexture> m_cbTexture;
};