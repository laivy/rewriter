module;

// Windows
#include <wrl.h>

// DirectX
#include <d3d12.h>
#include "External/DirectX/d3dx12.h"

module Library.Graphics.D3D:Util;

import std;
import Library.Graphics;
import Library.Graphics.Global;

using Microsoft::WRL::ComPtr;

namespace Graphics::D3D
{
	ComPtr<ID3D12Resource> CreateResourceBuffer(D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES state, UINT64 size)
	{
		ComPtr<ID3D12Resource> buffer;

		CD3DX12_HEAP_PROPERTIES heapProperties{ type };
		auto desc{ CD3DX12_RESOURCE_DESC::Buffer(size) };

		HRESULT hr{ E_FAIL };
		hr = g_d3dDevice->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			state,
			nullptr,
			IID_PPV_ARGS(&buffer)
		);

		if (FAILED(hr))
			return nullptr;

		return buffer;
	}

	bool CopyResource(const ComPtr<ID3D12Resource>& destination, std::span<const std::byte> source)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_HEAP_FLAGS heapFlags{};
		if (FAILED(destination->GetHeapProperties(&heapProperties, &heapFlags)))
			return false;

		switch (heapProperties.Type)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			auto uploadBuffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, source.size()) };

			D3D12_SUBRESOURCE_DATA subresourceData{};
			subresourceData.pData = source.data();
			subresourceData.RowPitch = source.size();
			subresourceData.SlicePitch = source.size();
			::UpdateSubresources(g_uploadCommandList.Get(), destination.Get(), uploadBuffer.Get(), 0, 0, 1, &subresourceData);

			g_uploadBuffers.push_back(std::move(uploadBuffer));
			break;
		}
		default:
			break;
		}

		return true;
	}

	bool CopyResource(const ComPtr<ID3D12Resource>& destination, const D3D12_SUBRESOURCE_DATA& subresource)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};
		D3D12_HEAP_FLAGS heapFlags{};
		if (FAILED(destination->GetHeapProperties(&heapProperties, &heapFlags)))
			return false;

		switch (heapProperties.Type)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			UINT64 size{ ::GetRequiredIntermediateSize(destination.Get(), 0, 1) };
			auto uploadBuffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, size) };
			::UpdateSubresources(g_uploadCommandList.Get(), destination.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);

			g_uploadBuffers.push_back(std::move(uploadBuffer));
			break;
		}
		default:
			break;
		}

		return true;
	}
}
