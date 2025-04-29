#include "Stdafx.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"

namespace Graphics::D3D
{
	DescriptorManager::DescriptorManager() :
		m_cpuSrvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
		m_gpuSrvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 },
#ifdef _IMGUI
		m_imGuiSrvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 },
#endif
		m_rtvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
		m_dsvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 }
	{
		if (s_srvDescriptorSize == 0)
			s_srvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		if (s_rtvDescriptorSize == 0)
			s_rtvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		if (s_dsvDescriptorSize == 0)
			s_dsvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
#ifdef _IMGUI
		m_imGuiDescriptor = m_imGuiSrvHeap.Allocate();
#endif
	}

	void DescriptorManager::SetDescriptorHeaps()
	{
		// CPU SRV 힙에 있는 것들을 GPU SRV 힙으로 복사
		m_gpuSrvHeap.Reserve(m_cpuSrvHeap.GetSize());
		g_d3dDevice->CopyDescriptorsSimple(
			m_cpuSrvHeap.GetSize(),
			m_gpuSrvHeap.GetHeap()->GetCPUDescriptorHandleForHeapStart(),
			m_cpuSrvHeap.GetHeap()->GetCPUDescriptorHandleForHeapStart(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
		g_commandList->SetDescriptorHeaps(1, m_gpuSrvHeap.GetHeap().GetAddressOf());
	}

	Descriptor* DescriptorManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return m_cpuSrvHeap.Allocate();
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return m_rtvHeap.Allocate();
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return m_dsvHeap.Allocate();
		default:
			assert(false);
			break;
		}
		return nullptr;
	}

	void DescriptorManager::Deallocate(D3D12_DESCRIPTOR_HEAP_TYPE type, Descriptor* descriptor)
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			m_cpuSrvHeap.Deallocate(descriptor);
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			m_rtvHeap.Deallocate(descriptor);
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			m_dsvHeap.Deallocate(descriptor);
			break;
		default:
			assert(false);
			break;
		}
	}

	ComPtr<ID3D12DescriptorHeap> DescriptorManager::GetImGuiSrvHeap() const
	{
		return m_imGuiSrvHeap.GetHeap();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorManager::GetImGuiSrvCpuHandle() const
	{
		return m_imGuiDescriptor->GetCpuHandle();
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorManager::GetImGuiSrvGpuHandle() const
	{
		return m_imGuiDescriptor->GetGpuHandle();
	}

	DescriptorManager::Heap::Heap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask) :
		m_desc{}
	{
		m_desc.Type = type;
		m_desc.NumDescriptors = numDescriptors;
		m_desc.Flags = flags;
		m_desc.NodeMask = nodeMask;
		g_d3dDevice->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_heap));
	}

	Descriptor* DescriptorManager::Heap::Allocate()
	{
		if (m_descriptors.size() >= m_desc.NumDescriptors)
			Reserve(m_desc.NumDescriptors + std::max(m_desc.NumDescriptors / 2, 1U));

		UINT descriptorSize{ GetDescriptorSize() };
		INT index{ static_cast<INT>(m_descriptors.size()) };
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_heap->GetCPUDescriptorHandleForHeapStart(), index, descriptorSize };
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{ m_heap->GetGPUDescriptorHandleForHeapStart(), index, descriptorSize };
		auto& descriptor{ m_descriptors.emplace_back(cpuHandle, gpuHandle) };
		return &descriptor;
	}

	void DescriptorManager::Heap::Deallocate(Descriptor* descriptor)
	{
		std::erase_if(m_descriptors, [descriptor](const auto& desc) { return &desc == descriptor; });
	}

	void DescriptorManager::Heap::Reserve(UINT size)
	{
		if (m_desc.NumDescriptors >= size)
			return;

		D3D12_DESCRIPTOR_HEAP_DESC desc{ m_desc };
		desc.NumDescriptors = static_cast<UINT>(size);

		ComPtr<ID3D12DescriptorHeap> heap;
		g_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
		if (m_desc.Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
			g_d3dDevice->CopyDescriptorsSimple(m_desc.NumDescriptors, heap->GetCPUDescriptorHandleForHeapStart(), m_heap->GetCPUDescriptorHandleForHeapStart(), m_desc.Type);
		m_desc.NumDescriptors = desc.NumDescriptors;
		m_heap.Swap(heap);

		UINT descriptorSize{ GetDescriptorSize() };
		for (size_t i{ 0 }; auto & descriptor : m_descriptors)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_heap->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(i), descriptorSize };
			CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{ m_heap->GetGPUDescriptorHandleForHeapStart(), static_cast<INT>(i), descriptorSize };
			descriptor = Descriptor{ cpuHandle, gpuHandle };
			++i;
		}
	}

	ComPtr<ID3D12DescriptorHeap> DescriptorManager::Heap::GetHeap() const
	{
		return m_heap;
	}

	UINT DescriptorManager::Heap::GetSize() const
	{
		return static_cast<UINT>(m_descriptors.size());
	}

	UINT DescriptorManager::Heap::GetDescriptorSize() const
	{
		UINT descriptorSize{ 0 };
		switch (m_desc.Type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			descriptorSize = s_srvDescriptorSize;
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			descriptorSize = s_rtvDescriptorSize;
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			descriptorSize = s_dsvDescriptorSize;
			break;
		default:
			assert(false);
			break;
		}
		return descriptorSize;
	}
}
