#pragma once

class Mesh
{
public:
	enum class Type
	{
		DEFAULT
	};

public:
	Mesh(const ComPtr<ID3D12Device>& d3dDevice, const ComPtr<ID3D12GraphicsCommandList>& commandList);
	~Mesh() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList);

	void ReleaseUploadBuffer();

private:
	UINT m_nVertices;
	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;
};