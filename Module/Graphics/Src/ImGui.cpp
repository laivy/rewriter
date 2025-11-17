#include "Pch.h"
#ifdef _IMGUI
#include <fstream>
#include "Context.h"
#include "Descriptor.h"
#include "../Include/Graphics/ImGui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
	Graphics::ImGui::Texture LoadTexture(std::span<char> binary)
	{
		static const Graphics::ImGui::Texture InvalidTexture{ .id = ImTextureID_Invalid };

		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return InvalidTexture;

		auto imguiCtx{ Graphics::ImGui::Context::GetInstance() };
		if (!imguiCtx)
			return InvalidTexture;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::unique_ptr<std::uint8_t[]> decodedData;
		D3D12_SUBRESOURCE_DATA subresource{};
		if (FAILED(DirectX::LoadWICTextureFromMemoryEx(
			ctx->d3d12Device.Get(),
			reinterpret_cast<std::uint8_t*>(binary.data()),
			binary.size(),
			0,
			D3D12_RESOURCE_FLAG_NONE,
			DirectX::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FORCE_RGBA32,
			&resource,
			decodedData,
			subresource)))
		{
			return InvalidTexture;
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
			return InvalidTexture;
		}

		// 데이터 복사 및 GPU 업로드 대기
		if (FAILED(imguiCtx->commandAllocator->Reset()))
			return InvalidTexture;
		if (FAILED(imguiCtx->commandList->Reset(imguiCtx->commandAllocator.Get(), nullptr)))
			return InvalidTexture;
		::UpdateSubresources(imguiCtx->commandList.Get(), resource.Get(), uploadBuffer.Get(), 0, 0, 1, &subresource);
		if (FAILED(imguiCtx->commandList->Close()))
			return InvalidTexture;

		const std::array<ID3D12CommandList*, 1> commandLists{ imguiCtx->commandList.Get() };
		ctx->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		if (FAILED(ctx->commandQueue->Signal(imguiCtx->fence.Get(), imguiCtx->fenceValue)))
			return InvalidTexture;
		if (imguiCtx->fence->GetCompletedValue() < imguiCtx->fenceValue)
		{
			if (FAILED(imguiCtx->fence->SetEventOnCompletion(imguiCtx->fenceValue, imguiCtx->fenceEvent)))
				return InvalidTexture;
			if (::WaitForSingleObject(imguiCtx->fenceEvent, INFINITE) == WAIT_FAILED)
				return InvalidTexture;
		}
		++imguiCtx->fenceValue;

		// SRV 생성
		D3D12_RESOURCE_DESC desc{ resource->GetDesc() };
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;

		auto handle{ ctx->descriptorManager->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
		ctx->d3d12Device->CreateShaderResourceView(resource.Get(), &srvDesc, handle.cpuHandle);

		// 캐싱
		Graphics::ImGui::Texture texture{};
		texture.resource = resource;
		texture.id = handle.gpuHandle.ptr;
		texture.width = desc.Width;
		texture.height = desc.Height;
		imguiCtx->textures.emplace(L"todo", texture);
		return texture;
	}
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

	ImTextureID CreateTexture(const std::filesystem::path& path)
	{
		std::ifstream file{ path, std::ios::binary };
		if (!file)
			return ImTextureID_Invalid;

		std::vector<char> binary{ std::istreambuf_iterator<char>(file), {} };
		return CreateTexture(path.wstring(), binary);
	}

	ImTextureID CreateTexture(const std::wstring& name, std::span<char> binary)
	{
		auto ictx{ Graphics::ImGui::Context::GetInstance() };
		if (!ictx)
			return ImTextureID_Invalid;

		if (ictx->textures.contains(name))
			return ictx->textures.at(name).id;

		auto texture{ LoadTexture(binary) };
		ictx->textures.emplace(name, texture);
		return texture.id;
	}

	ImTextureID GetTexture(Resource::ID id)
	{
		auto path{ Resource::GetPath(id) };
		if (!path)
			return ImTextureID_Invalid;
		return GetTexture(*path);
	}

	ImTextureID GetTexture(const std::wstring& name)
	{
		auto ictx{ Context::GetInstance() };
		if (!ictx)
			return ImTextureID_Invalid;
		if (!ictx->textures.contains(name))
			return ImTextureID_Invalid;
		return ictx->textures.at(name).id;
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
}
#endif
