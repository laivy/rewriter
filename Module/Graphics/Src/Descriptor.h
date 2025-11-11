#pragma once

namespace Graphics::Descriptor
{
	struct Handle
	{
		D3D12_DESCRIPTOR_HEAP_TYPE type;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	};

	struct Heap
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		std::vector<std::optional<Handle>> handles;
	};

	class Manager
	{
	public:
		Manager();

		Handle Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type);
		void Free(const Handle& handle);

		ID3D12DescriptorHeap* GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

	private:
		std::array<UINT, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_descriptorSizes;
		std::array<Heap, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> m_heaps;
	};
}
