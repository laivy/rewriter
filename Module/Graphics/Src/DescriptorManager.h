#pragma once

namespace Graphics::D3D
{
	class Descriptor;

	class DescriptorManager : public TSingleton<DescriptorManager>
	{
	private:
		class Heap
		{
		public:
			Heap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask);
			~Heap() = default;

			Descriptor* Allocate();
			void Deallocate(Descriptor* descriptor);

			ComPtr<ID3D12DescriptorHeap> GetHeap() const;

		private:
			UINT GetDescriptorSize() const;

		private:
			ComPtr<ID3D12DescriptorHeap> m_heap;
			D3D12_DESCRIPTOR_HEAP_DESC m_desc;
			std::vector<Descriptor> m_descriptors;
			std::vector<INT> m_freeList;
		};

	public:
		DescriptorManager();
		~DescriptorManager() = default;

		void SetDescriptorHeaps();

		Descriptor* Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type);
		void Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE type, Descriptor* descriptor);

#ifdef _IMGUI
		ComPtr<ID3D12DescriptorHeap> GetSrvHeap() const;
#endif

	private:
		static inline UINT s_srvDescriptorSize{ 0 };
		static inline UINT s_rtvDescriptorSize{ 0 };
		static inline UINT s_dsvDescriptorSize{ 0 };

		Heap m_srvHeap;
		Heap m_rtvHeap;
		Heap m_dsvHeap;
	};
}
