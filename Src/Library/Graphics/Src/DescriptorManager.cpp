#include "Pch.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"

namespace Graphics::D3D
{
	DescriptorManager::DescriptorManager() :
		m_srvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 },
		m_rtvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
		m_dsvHeap{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 512, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 }
	{
		if (s_srvDescriptorSize == 0)
			s_srvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		if (s_rtvDescriptorSize == 0)
			s_rtvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		if (s_dsvDescriptorSize == 0)
			s_dsvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	void DescriptorManager::SetDescriptorHeaps()
	{
		g_commandList->SetDescriptorHeaps(1, m_srvHeap.GetHeap().GetAddressOf());
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

#ifdef _IMGUI
	ComPtr<ID3D12DescriptorHeap> DescriptorManager::GetSrvHeap() const
	{
		return m_srvHeap.GetHeap();
	}
#endif

	DescriptorManager::Heap::Heap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT nodeMask) :
		m_desc{}
	{
		m_desc.Type = type;
		m_desc.NumDescriptors = numDescriptors;
		m_desc.Flags = flags;
		m_desc.NodeMask = nodeMask;
		g_d3dDevice->CreateDescriptorHeap(&m_desc, IID_PPV_ARGS(&m_heap));

		m_descriptors.resize(numDescriptors);
		m_freeList.reserve(numDescriptors);
		for (int i : std::views::iota(0)
				   | std::views::take(numDescriptors)
				   | std::views::reverse)
		{
			m_freeList.push_back(i);
		}
	}

	Descriptor* DescriptorManager::Heap::Allocate()
	{
		assert(!m_freeList.empty() && "NO AVAILABLE HEAP SPACE");

		INT index{ m_freeList.back() };
		m_freeList.pop_back();

		UINT descriptorSize{ GetDescriptorSize() };
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle{ m_heap->GetCPUDescriptorHandleForHeapStart(), index, descriptorSize };
		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{ D3D12_DEFAULT };
		if (m_desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			gpuHandle.InitOffsetted(m_heap->GetGPUDescriptorHandleForHeapStart(), index, descriptorSize);
		m_descriptors[index] = Descriptor{ cpuHandle, gpuHandle };
		return &m_descriptors[index];
	}

	void DescriptorManager::Heap::Deallocate(Descriptor* descriptor)
	{
		auto it{ std::ranges::find_if(m_descriptors, [descriptor](const auto& desc) { return &desc == descriptor; }) };
		if (it == m_descriptors.end())
			return;

		*it = Descriptor{};

		INT index{ static_cast<INT>(std::distance(m_descriptors.begin(), it)) };
		m_freeList.push_back(index);
	}

	ComPtr<ID3D12DescriptorHeap> DescriptorManager::Heap::GetHeap() const
	{
		return m_heap;
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
