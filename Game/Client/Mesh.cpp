#include "Stdafx.h"
#include "App.h"
#include "Mesh.h"
#include "Renderer3D.h"

Mesh::Mesh() : 
	m_primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_POINTLIST }, 
	m_nVertices{ 1 }
{
	auto d3dDevice{ Renderer3D::d3dDevice };
	auto commandList{ Renderer3D::commandList };

	struct Vertex
	{
		float x, y, z;
	};

	// 기본적으로 메쉬는 점 하나만을 가짐
	constexpr Vertex vertex{ 0.0f, 0.0f, 0.0f };
	constexpr UINT bufferSize{ sizeof(vertex) };

	// 디폴트 버퍼 생성
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_COMMON,
		NULL,
		IID_PPV_ARGS(&m_vertexBuffer)));

	// 업로드 버퍼 생성
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&m_vertexUploadBuffer)));

	// 업로드 버퍼 -> 디폴트 버퍼로 복사
	D3D12_SUBRESOURCE_DATA bufferData{};
	bufferData.pData = &vertex;
	bufferData.RowPitch = bufferSize;
	bufferData.SlicePitch = bufferData.RowPitch;
	UpdateSubresources(commandList.Get(), m_vertexBuffer.Get(), m_vertexUploadBuffer.Get(), 0, 0, 1, &bufferData);

	// 리소스 베리어 설정
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

	// 정점 버퍼 뷰
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = bufferSize;
	m_vertexBufferView.StrideInBytes = bufferSize / m_nVertices;
}

void Mesh::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->IASetPrimitiveTopology(m_primitiveTopology);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

void Mesh::ReleaseUploadBuffer()
{
	m_vertexUploadBuffer.Reset();
}
