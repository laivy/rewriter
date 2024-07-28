#include "Stdafx.h"
#include "Renderer.h"
#include "Renderer3D.h"

namespace Renderer3D
{
	void Begin()
	{
		DX::ThrowIfFailed(Renderer::commandAllocators[Renderer::frameIndex]->Reset());
		DX::ThrowIfFailed(Renderer::commandList->Reset(Renderer::commandAllocators[Renderer::frameIndex].Get(), nullptr));

		Renderer::commandList->SetGraphicsRootSignature(Renderer::rootSignature.Get());
		Renderer::commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(Renderer::renderTargets[Renderer::frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ Renderer::rtvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(Renderer::frameIndex), Renderer::rtvDescriptorSize };
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ Renderer::dsvHeap->GetCPUDescriptorHandleForHeapStart() };
		Renderer::commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		Renderer::commandList->RSSetViewports(1, &Renderer::viewport);
		Renderer::commandList->RSSetScissorRects(1, &Renderer::scissorRect);

		constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		Renderer::commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		Renderer::commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

		//if (auto srvDescHeap{ ResourceManager::GetInstance()->GetSrvDescriptorHeap() })
		//	m_commandList->SetDescriptorHeaps(1, srvDescHeap);
	}

	void End()
	{
		DX::ThrowIfFailed(Renderer::commandList->Close());
		ID3D12CommandList* ppCommandList[]{ Renderer::commandList.Get() };
		Renderer::commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	}

	void DrawImage(const std::shared_ptr<Resource::PNG>& image, const INT2& position, float opacity)
	{
		
	}
}