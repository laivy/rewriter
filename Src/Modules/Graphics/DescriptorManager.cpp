#include "Stdafx.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"

namespace Graphics::D3D
{
	DescriptorManager::DescriptorManager() :
		m_srvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 },
		m_rtvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
		m_dsvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 }
	{
		if (s_srvDescriptorSize == 0)
			s_srvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		if (s_rtvDescriptorSize == 0)
			s_rtvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		if (s_dsvDescriptorSize == 0)
			s_dsvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	Descriptor* DescriptorManager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return m_srvHeap.Allocate();
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
			m_srvHeap.Deallocate(descriptor);
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

	DescriptorManager::Heap::Heap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask) :
		m_desc{}
	{
		m_desc.Type = type;
		m_desc.NumDescriptors = numDescriptors;
		m_desc.Flags = flags;
		m_desc.NodeMask = nodeMask;
		g_d3dDevice->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_heap));

		m_descriptors.reserve(numDescriptors);
	}

	Descriptor* DescriptorManager::Heap::Allocate()
	{
		// 크기 확장
		if (m_descriptors.size() >= m_desc.NumDescriptors)
		{
			ComPtr<ID3D12DescriptorHeap> heap;
			D3D12_DESCRIPTOR_HEAP_DESC desc{ m_desc };
			desc.NumDescriptors += m_desc.NumDescriptors / 2;
			g_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
			g_d3dDevice->CopyDescriptorsSimple(m_desc.NumDescriptors, heap->GetCPUDescriptorHandleForHeapStart(), m_heap->GetCPUDescriptorHandleForHeapStart(), m_desc.Type);

			m_heap.Swap(heap);
			m_descriptors.resize(desc.NumDescriptors);
		}

		INT index{ static_cast<INT>(m_descriptors.size()) };
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

		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_heap->GetCPUDescriptorHandleForHeapStart(), index, descriptorSize };
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{ m_heap->GetGPUDescriptorHandleForHeapStart(), index, descriptorSize };
		auto& descriptor{ m_descriptors.emplace_back(cpuHandle, gpuHandle) };
		return &descriptor;
	}

	void DescriptorManager::Heap::Deallocate(Descriptor* descriptor)
	{
		std::erase_if(m_descriptors, [descriptor](const auto& desc) { return &desc == descriptor; });
	}
}
