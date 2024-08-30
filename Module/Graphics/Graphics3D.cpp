#include "Stdafx.h"
#include "Globals.h"
#include "Graphics3D.h"

namespace Graphics::D3D
{
	DLL_API bool Begin()
	{
		if (FAILED(commandAllocators[frameIndex]->Reset()))
			return false;
		if (FAILED(commandList->Reset(commandAllocators[frameIndex].Get(), nullptr)))
			return false;

		commandList->SetGraphicsRootSignature(rootSignature.Get());
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ rtvDescHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(frameIndex), rtvDescriptorSize };
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ dsvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissorRect);

		constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		return true;
	}

	DLL_API bool End()
	{
		if (FAILED(commandList->Close()))
			return false;

		ID3D12CommandList* ppCommandList[]{ commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
		return true;
	}
}
