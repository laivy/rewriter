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
			void Reserve(UINT size);

			ComPtr<ID3D12DescriptorHeap> GetHeap() const;
			UINT GetSize() const;

		private:
			UINT GetDescriptorSize() const;

		private:
			ComPtr<ID3D12DescriptorHeap> m_heap;
			D3D12_DESCRIPTOR_HEAP_DESC m_desc;
			std::list<Descriptor> m_descriptors;
		};

	public:
		DescriptorManager();
		~DescriptorManager() = default;

		void SetDescriptorHeaps();

		Descriptor* Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type);
		void Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE type, Descriptor* descriptor);

#ifdef _IMGUI
		ComPtr<ID3D12DescriptorHeap> GetImGuiSrvHeap() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetImGuiSrvCpuHandle() const;
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetImGuiSrvGpuHandle() const;
#endif

	private:
		static inline UINT s_srvDescriptorSize{ 0 };
		static inline UINT s_rtvDescriptorSize{ 0 };
		static inline UINT s_dsvDescriptorSize{ 0 };

		Heap m_cpuSrvHeap;
		Heap m_gpuSrvHeap;
		Heap m_rtvHeap;
		Heap m_dsvHeap;
#ifdef _IMGUI
		Heap m_imGuiSrvHeap;
		Descriptor* m_imGuiDescriptor;
#endif
	};
}
