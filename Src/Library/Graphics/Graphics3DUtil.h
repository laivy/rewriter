#pragma once

namespace Graphics::D3D
{
	ComPtr<ID3D12Resource> CreateResourceBuffer(D3D12_HEAP_TYPE type, D3D12_RESOURCE_STATES state, UINT64 size);
	bool CopyResource(const ComPtr<ID3D12Resource>& destination, std::span<const std::byte> source);
	bool CopyResource(const ComPtr<ID3D12Resource>& destination, const D3D12_SUBRESOURCE_DATA& subresource);
}
