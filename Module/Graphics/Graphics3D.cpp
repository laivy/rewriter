#include "Stdafx.h"
#include "Global.h"
#include "Graphics3D.h"
#include "External/DirectX/DDSTextureLoader12.h"

namespace Graphics::D3D
{
	static void OnTextureDestroy(Resource::Texture* texture)
	{

	}

	DLL_API bool Begin()
	{
		if (FAILED(g_commandAllocators[g_frameIndex]->Reset()))
			return false;
		if (FAILED(g_commandList->Reset(g_commandAllocators[g_frameIndex].Get(), nullptr)))
			return false;

		g_commandList->SetGraphicsRootSignature(g_rootSignature.Get());
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ g_rtvDescHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(g_frameIndex), g_rtvDescriptorSize };
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ g_dsvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		g_commandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
		g_commandList->RSSetViewports(1, &g_viewport);
		g_commandList->RSSetScissorRects(1, &g_scissorRect);

		constexpr float clearColor[]{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		g_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, NULL);
		g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		return true;
	}

	DLL_API bool End()
	{
#if !defined _CLIENT && !defined _TOOL
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
#endif
		if (FAILED(g_commandList->Close()))
			return false;

		ID3D12CommandList* ppCommandList[]{ g_commandList.Get() };
		g_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
		return true;
	}

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary)
	{
		ComPtr<ID3D12Resource> resource;
		std::vector<D3D12_SUBRESOURCE_DATA> subresource{};
		if (FAILED(DirectX::LoadDDSTextureFromMemory(g_d3dDevice.Get(), reinterpret_cast<const uint8_t*>(binary.data()), binary.size(), &resource, subresource)))
			return nullptr;

		auto texture{ std::make_shared<Resource::Texture>(resource, &OnTextureDestroy) };
		return texture;
	}

	void CreateResourceFromTexture(const std::shared_ptr<Resource::Texture>& texture)
	{
	}
}
