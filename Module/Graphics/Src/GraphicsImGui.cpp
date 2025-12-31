#include "Pch.h"
#ifdef _IMGUI
#include "Context.h"
#include "Descriptor.h"
#include "GraphicsImGui.h"
#include <Common/Delegate.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
	Graphics::ImGui::Texture CreateTexture(Resource::ID id)
	{
		const Graphics::ImGui::Texture Empty{};

		auto sprite{ Resource::GetSprite(id) };
		if (!sprite)
			return Empty;
		if (sprite->binary.empty())
			return Empty;

		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return Empty;

		auto ictx{ Graphics::ImGui::Context::GetInstance() };
		if (!ictx)
			return Empty;

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
			return Empty;
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
			return Empty;
		}

		// 데이터 복사 및 GPU 업로드 대기
		if (FAILED(ictx->commandAllocator->Reset()))
			return Empty;
		if (FAILED(ictx->commandList->Reset(ictx->commandAllocator.Get(), nullptr)))
			return Empty;
		::UpdateSubresources(ictx->commandList.Get(), resource.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);
		if (FAILED(ictx->commandList->Close()))
			return Empty;

		const std::array<ID3D12CommandList*, 1> commandLists{ ictx->commandList.Get() };
		ctx->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		if (FAILED(ctx->commandQueue->Signal(ictx->fence.Get(), ictx->fenceValue)))
			return Empty;
		if (ictx->fence->GetCompletedValue() < ictx->fenceValue)
		{
			if (FAILED(ictx->fence->SetEventOnCompletion(ictx->fenceValue, ictx->fenceEvent)))
				return Empty;
			if (::WaitForSingleObject(ictx->fenceEvent, INFINITE) == WAIT_FAILED)
				return Empty;
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
		texture.size.x = desc.Width;
		texture.size.y = desc.Height;
		return texture;
	}

	Delegate<> OnPreBegin;
}

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
		OnPreBegin.Broadcast();
		OnPreBegin.Unbind();

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
	std::pair<ImTextureID, ImVec2> Image(Resource::ID id)
	{
		constexpr std::pair<ImTextureID, ImVec2> Empty{};

		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return Empty;

		if (ctx->textures.contains(id))
		{
			const auto& texture{ ctx->textures.at(id) };
			return { texture.id, texture.size };
		}

		ctx->textures.emplace(id, CreateTexture(id));
		return Image(id);
	}

	std::pair<ImTextureID, ImVec2> Image(const std::wstring& path)
	{
		return Image(Resource::Get(path));
	}

	void DeleteImage(Resource::ID id)
	{
		OnPreBegin.Bind([id]()
		{
			if (auto ctx{ Context::GetInstance() })
				ctx->textures.erase(id);
		});
	}
#endif
}
#endif
