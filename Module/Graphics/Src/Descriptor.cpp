#include "Pch.h"
#include "Context.h"
#include "Descriptor.h"

namespace Graphics::Descriptor
{
	Manager::Manager() :
		m_descriptorSizes{}
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device)
			return;

		for (UINT type{ 0 }; type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++type)
			m_descriptorSizes[type] = ctx->d3d12Device->GetDescriptorHandleIncrementSize(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(type));

		auto& srvHeap{ m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] };
		srvHeap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeap.desc.NumDescriptors = 1024;
		srvHeap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeap.desc.NodeMask = 0;
		if (FAILED(ctx->d3d12Device->CreateDescriptorHeap(&srvHeap.desc, IID_PPV_ARGS(&srvHeap.heap))))
			return;

		auto& samplerHeap{ m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] };
		samplerHeap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHeap.desc.NumDescriptors = 1;
		samplerHeap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		samplerHeap.desc.NodeMask = 0;
		if (FAILED(ctx->d3d12Device->CreateDescriptorHeap(&samplerHeap.desc, IID_PPV_ARGS(&samplerHeap.heap))))
			return;

		auto& rtvHeap{ m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] };
		rtvHeap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeap.desc.NumDescriptors = 32;
		rtvHeap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeap.desc.NodeMask = 0;
		if (FAILED(ctx->d3d12Device->CreateDescriptorHeap(&rtvHeap.desc, IID_PPV_ARGS(&rtvHeap.heap))))
			return;

		auto& dsvHeap{ m_heaps[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] };
		dsvHeap.desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeap.desc.NumDescriptors = 32;
		dsvHeap.desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeap.desc.NodeMask = 0;
		if (FAILED(ctx->d3d12Device->CreateDescriptorHeap(&dsvHeap.desc, IID_PPV_ARGS(&dsvHeap.heap))))
			return;
	}

	Handle Manager::Allocate(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		auto& heap{ m_heaps[type] };
		auto it{ std::ranges::find_if(heap.handles, [](const auto& handle) { return !handle.has_value(); }) };
		if (it == heap.handles.end())
			return {};

		auto& handle{ it->emplace() };
		handle.type = type;

		const INT index{ static_cast<INT>(std::distance(heap.handles.begin(), it)) };
		handle.cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE{ heap.heap->GetCPUDescriptorHandleForHeapStart(), index, m_descriptorSizes[type] };
		if (heap.desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			handle.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE{ heap.heap->GetGPUDescriptorHandleForHeapStart(), index, m_descriptorSizes[type] };
		else
			handle.gpuHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE{ D3D12_DEFAULT };
		return handle;
	}

	void Manager::Free(const Handle& handle)
	{
		auto& heap{ m_heaps[handle.type] };
		const INT index{ static_cast<INT>((handle.cpuHandle.ptr - heap.heap->GetCPUDescriptorHandleForHeapStart().ptr) / m_descriptorSizes.at(handle.type)) };
		if (index < 0 || index >= static_cast<INT>(heap.handles.size()))
			return;
		heap.handles[index].reset();
	}

	ID3D12DescriptorHeap* Manager::GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const
	{
		return m_heaps.at(type).heap.Get();
	}
}
