#include "Pch.h"
#ifdef _IMGUI
#include <fstream>
#include "Context.h"
#include "Descriptor.h"
#include "../Include/Graphics/ImGui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Graphics::ImGui
{
	ImGuiContext* GetContext()
	{
		return ::ImGui::GetCurrentContext();
	}

	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return ::ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	}

	void Begin()
	{
		if (auto ictx{ Context::GetInstance() })
		{
			for (const auto& [id, texture] : ictx->textureHolder)
				ictx->textures[id] = texture;
			ictx->textureHolder.clear();
		}

		::ImGui_ImplDX12_NewFrame();
		::ImGui_ImplWin32_NewFrame();
		::ImGui::NewFrame();
	}

	void End()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return;

		::ImGui::Render();
		::ImGui_ImplDX12_RenderDrawData(::ImGui::GetDrawData(), ctx->commandList.Get());
		if (::ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
	}

#ifdef _TOOL
	ImTextureID CreateTexture(Resource::ID id)
	{
		auto sprite{ Resource::GetSprite(id) };
		if (!sprite)
			return ImTextureID_Invalid;
		if (sprite->binary.empty())
			return ImTextureID_Invalid;

		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return ImTextureID_Invalid;

		auto ictx{ Graphics::ImGui::Context::GetInstance() };
		if (!ictx)
			return ImTextureID_Invalid;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::unique_ptr<std::uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource{};
		if (FAILED(DirectX::LoadWICTextureFromMemoryEx(
			ctx->d3d12Device.Get(),
			reinterpret_cast<std::uint8_t*>(sprite->binary.data()),
			sprite->binary.size(),
			0,
			D3D12_RESOURCE_FLAG_NONE,
			DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32,
			&resource,
			decodedData,
			subresource)))
		{
			return ImTextureID_Invalid;
		}

		// 업로드 버퍼 생성
		CD3DX12_HEAP_PROPERTIES uploadBufferProp{ D3D12_HEAP_TYPE_UPLOAD };
		UINT64 uploadBufferSize{ ::GetRequiredIntermediateSize(resource.Get(), 0, 1) };
		CD3DX12_RESOURCE_DESC uploadBufferDesc{ CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize) };
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		if (FAILED(ctx->d3d12Device->CreateCommittedResource(
			&uploadBufferProp,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer))))
		{
			return ImTextureID_Invalid;
		}

		// 데이터 복사 및 GPU 업로드 대기
		if (FAILED(ictx->commandAllocator->Reset()))
			return ImTextureID_Invalid;
		if (FAILED(ictx->commandList->Reset(ictx->commandAllocator.Get(), nullptr)))
			return ImTextureID_Invalid;
		::UpdateSubresources(ictx->commandList.Get(), resource.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);
		if (FAILED(ictx->commandList->Close()))
			return ImTextureID_Invalid;

		const std::array<ID3D12CommandList*, 1> commandLists{ ictx->commandList.Get() };
		ctx->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		if (FAILED(ctx->commandQueue->Signal(ictx->fence.Get(), ictx->fenceValue)))
			return ImTextureID_Invalid;
		if (ictx->fence->GetCompletedValue() < ictx->fenceValue)
		{
			if (FAILED(ictx->fence->SetEventOnCompletion(ictx->fenceValue, ictx->fenceEvent)))
				return ImTextureID_Invalid;
			if (::WaitForSingleObject(ictx->fenceEvent, INFINITE) == WAIT_FAILED)
				return ImTextureID_Invalid;
		}
		++ictx->fenceValue;

		// SRV 생성
		D3D12_RESOURCE_DESC desc{ resource->GetDesc() };
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;

		auto handle{ ctx->descriptorManager->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
		ctx->d3d12Device->CreateShaderResourceView(resource.Get(), &srvDesc, handle.cpuHandle);

		Graphics::ImGui::Texture texture{};
		texture.resource = resource;
		texture.id = handle.gpuHandle.ptr;
		texture.width = desc.Width;
		texture.height = desc.Height;

		// 이미 텍스쳐가 있는 경우 이번 프레임에 사용되고 있을 수도 있으므로
		// 다음 프레임 시작할 때 갱신
		if (ictx->textures.contains(id))
		{
			ictx->textureHolder.emplace_back(id, texture);
			return ictx->textures.at(id).id;
		}

		ictx->textures.emplace(id, texture);
		return texture.id;
	}

	ImTextureID GetTexture(Resource::ID id)
	{
		auto ictx{ Context::GetInstance() };
		if (!ictx)
			return ImTextureID_Invalid;
		if (ictx->textures.contains(id))
			return ictx->textures.at(id).id;
		return CreateTexture(id);
	}

	ImTextureID GetTexture(const std::wstring& name)
	{
		const auto id{ Resource::Get(name) };
		if (id == Resource::InvalidID)
			return ImTextureID_Invalid;
		return GetTexture(id);
	}

	ImVec2 GetTextureSize(ImTextureID id)
	{
		auto ctx{ Graphics::ImGui::Context::GetInstance() };
		if (!ctx)
			return ImVec2{ 0.0f, 0.0f };

		for (const auto& texture : ctx->textures | std::views::values)
		{
			if (texture.id == id)
				return ImVec2{ static_cast<float>(texture.width), static_cast<float>(texture.height) };
		}
		return ImVec2{ 0.0f, 0.0f };
	}
#endif
}
#endif
