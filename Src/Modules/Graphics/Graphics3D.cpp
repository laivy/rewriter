#include "Stdafx.h"
#include "Camera.h"
#include "Global.h"
#include "Graphics3D.h"
#include "Graphics3DUtil.h"
#include "External/DirectX/DDSTextureLoader12.h"

namespace Graphics::D3D
{
	struct VertexBuffer
	{
		ComPtr<ID3D12Resource> buffer;
		D3D12_VERTEX_BUFFER_VIEW view;
	};

	struct IndexBuffer
	{
		ComPtr<ID3D12Resource> buffer;
		D3D12_INDEX_BUFFER_VIEW view;
	};

	static void OnTextureDestroy(Resource::Texture* texture)
	{

	}

	static std::unique_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<Resource::Model::Vertex>& vertices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, sizeof(Resource::Model::Vertex) * vertices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(vertices.data()), sizeof(Resource::Model::Vertex) * vertices.size() });

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = buffer->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(sizeof(Resource::Model::Vertex) * vertices.size());
		view.StrideInBytes = sizeof(Resource::Model::Vertex);

		return std::make_unique<VertexBuffer>(buffer, view);
	}

	static std::unique_ptr<IndexBuffer> CreateIndexBuffer(const std::vector<int>& indices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, sizeof(int) * indices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(indices.data()), sizeof(int) * indices.size() });

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = buffer->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(sizeof(int) * indices.size());
		view.Format = DXGI_FORMAT_R32_UINT;

		return std::make_unique<IndexBuffer>(buffer, view);
	}

	DLL_API bool Begin()
	{
		// 이전 프레임에 복사가 완료됐을 것이므로 업로드 버퍼 해제
		g_uploadBuffers.clear();

		if (FAILED(g_commandAllocators[g_frameIndex]->Reset()))
			return false;
		if (FAILED(g_commandList->Reset(g_commandAllocators[g_frameIndex].Get(), nullptr)))
			return false;

		g_commandList->SetGraphicsRootSignature(g_rootSignature.Get());
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ g_rtvDescHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(g_frameIndex), g_rtvDescriptorSize };
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ g_dsvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		g_commandList->RSSetViewports(1, &g_viewport);
		g_commandList->RSSetScissorRects(1, &g_scissorRect);

		constexpr std::array clearColor{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		g_commandList->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
		g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		return true;
	}

	DLL_API bool End()
	{
#if !defined _CLIENT && !defined _TOOL
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
#endif
		if (FAILED(g_commandList->Close()))
			return false;

		std::array<ID3D12CommandList*, 1> commandLists{ g_commandList.Get() };
		g_commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		return true;
	}

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary)
	{
		ComPtr<ID3D12Resource> resource;
		std::vector<D3D12_SUBRESOURCE_DATA> subresource{};
		if (FAILED(DirectX::LoadDDSTextureFromMemory(g_d3dDevice.Get(), reinterpret_cast<const uint8_t*>(binary.data()), binary.size(), &resource, subresource)))
			return nullptr;

		auto texture{ std::make_shared<Resource::Texture>(resource, &OnTextureDestroy) };
		return texture;
	}

	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary)
	{
		size_t pos{ 0 };
		auto readBytes = [&binary, &pos](auto* dest, size_t size)
			{
				std::memcpy(dest, binary.data() + pos, size);
				pos += size;
			};

		auto read = [&readBytes](auto* dest)
			{
				constexpr auto size{ sizeof(*dest) };
				readBytes(dest, size);
			};

		auto model{ std::make_shared<Resource::Model>() };

		uint32_t meshCount{};
		read(&meshCount);
		model->meshes.resize(meshCount);

		for (auto& mesh : model->meshes)
		{
			uint32_t vertexCount{};
			read(&vertexCount);

			std::vector<Resource::Model::Vertex> vertices(vertexCount);
			for (auto& vertex : vertices)
				read(&vertex);

			uint32_t indexCount{};
			read(&indexCount);

			std::vector<int> indices(indexCount);
			readBytes(indices.data(), sizeof(int) * indexCount);

			auto vertexBuffer{ CreateVertexBuffer(vertices) };
			mesh.vertexBuffer = { vertexBuffer.release(), [](void* pointer) { delete static_cast<VertexBuffer*>(pointer); } };

			auto indexBuffer{ CreateIndexBuffer(indices) };
			mesh.indexBuffer = { indexBuffer.release(), [](void* pointer) { delete static_cast<IndexBuffer*>(pointer); } };

#ifdef _TOOL
			mesh.vertices = std::move(vertices);
			mesh.indices = std::move(indices);
#endif
		}

		assert(pos == binary.size());
		return model;
	}

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model)
	{
		for (const auto& mesh : model->meshes)
		{
			auto vertexBuffer{ static_cast<VertexBuffer*>(mesh.vertexBuffer.get()) };
			auto indexBuffer{ static_cast<IndexBuffer*>(mesh.indexBuffer.get()) };

			UINT indexCount{};
			switch (indexBuffer->view.Format)
			{
			case DXGI_FORMAT_R16_UINT:
				assert(indexBuffer->view.SizeInBytes % sizeof(uint16_t) == 0);
				indexCount = indexBuffer->view.SizeInBytes / sizeof(uint16_t);
				break;
			case DXGI_FORMAT_R32_UINT:
				assert(indexBuffer->view.SizeInBytes % sizeof(uint32_t) == 0);
				indexCount = indexBuffer->view.SizeInBytes / sizeof(uint32_t);
				break;
			default:
				assert(false && "INVALID INDEX BUFFER FORMAT");
				continue;
			}

			g_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_commandList->IASetVertexBuffers(0, 1, &vertexBuffer->view);
			g_commandList->IASetIndexBuffer(&indexBuffer->view);
			g_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		}
	}

	void CreateResourceFromTexture(const std::shared_ptr<Resource::Texture>& texture)
	{
	}
}
