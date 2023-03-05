#pragma once
#include "ConstantBuffer.h"

class NytImage
{
public:
	NytImage(ID3D12Resource* resource);
	~NytImage() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, RootParamIndex rootParameterIndex);

	ID3D12Resource* GetResource() const;

private:
	ComPtr<ID3D12Resource> m_resource;
	struct cbImage
	{
		FLOAT2 size;
		FLOAT2 dummy;
	};
	ConstantBuffer<cbImage> m_cbImage;
};