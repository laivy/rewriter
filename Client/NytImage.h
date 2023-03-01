#pragma once

class NytImage
{
public:
	NytImage(const ComPtr<ID3D12Resource>& resource);
	~NytImage() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, UINT rootParameterIndex);

	ID3D12Resource* GetResource() const;

private:
	ComPtr<ID3D12Resource> m_resource;
	FLOAT2 m_size;
};