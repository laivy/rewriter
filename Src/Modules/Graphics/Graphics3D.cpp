#include "Stdafx.h"
#include "Camera.h"
#include "Global.h"
#include "Graphics3D.h"
#include "Graphics3DUtil.h"
#include "PipelineState.h"
#include "External/DirectX/DDSTextureLoader12.h"

namespace Graphics::D3D
{
	struct VertexBuffer
	{
		ComPtr<ID3D12Resource> buffer;
		D3D12_VERTEX_BUFFER_VIEW view;
	};

	struct IndexBuffer
	{
		ComPtr<ID3D12Resource> buffer;
		D3D12_INDEX_BUFFER_VIEW view;
	};

	static bool CreateDXGIFactory()
	{
		UINT flags{ 0 };
#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		if (FAILED(::CreateDXGIFactory2(flags, IID_PPV_ARGS(&g_dxgiFactory))))
			return false;
		if (FAILED(g_dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER)))
			return false;
		return true;
	}

	static bool CreateD3DDevice()
	{
		ComPtr<IDXGIAdapter1> adapter;
		for (UINT i = 0; g_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 adapterDesc{};
			adapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;
			if (SUCCEEDED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_d3dDevice))))
				break;
		}
		if (!g_d3dDevice)
		{
			g_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			if (FAILED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_d3dDevice))))
				return false;
		}
		g_cbvSrvUavDescriptorIncrementSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		return true;
	}

	static bool CreateCommandQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(g_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue))))
			return false;
		return true;
	}

	static bool CreateSwapChain()
	{
		RECT rect{};
		::GetClientRect(g_hWnd, &rect);
		g_renderTargetSize = Int2{ rect.right - rect.left, rect.bottom - rect.top };

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = FRAME_COUNT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1> swapChain1;
		if (FAILED(g_dxgiFactory->CreateSwapChainForHwnd(g_commandQueue.Get(), g_hWnd, &swapChainDesc, nullptr, nullptr, &swapChain1)))
			return false;
		if (FAILED(swapChain1.As(&g_swapChain)))
			return false;
		g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
		return true;
	}

	static bool CreateRtvDsvSrvDescriptorHeap()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.NumDescriptors = FRAME_COUNT;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(g_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_rtvDescHeap))))
			return false;

		g_rtvDescriptorSize = g_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(g_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&g_dsvDescHeap))))
			return false;

		/*
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_srvDescHeap))))
			return false;
		*/

		return true;
	}

	static bool CreateRenderTargetView()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ g_rtvDescHeap->GetCPUDescriptorHandleForHeapStart() };
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			if (FAILED(g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i]))))
				return false;
			g_d3dDevice->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(g_rtvDescriptorSize);
		}
		return true;
	}

	static bool CreateDepthStencilView()
	{
		RECT rect{};
		::GetClientRect(g_hWnd, &rect);

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = rect.right - rect.left;
		desc.Height = rect.bottom - rect.top;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearValue{};
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;

		CD3DX12_HEAP_PROPERTIES prop{ D3D12_HEAP_TYPE_DEFAULT };
		if (FAILED(g_d3dDevice->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&g_depthStencil))))
			return false;

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;
		g_d3dDevice->CreateDepthStencilView(g_depthStencil.Get(), &depthStencilViewDesc, g_dsvDescHeap->GetCPUDescriptorHandleForHeapStart());
		return true;
	}

	static bool CreateRootSignature()
	{
		//std::array<CD3DX12_DESCRIPTOR_RANGE, 1> ranges{};
		//ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

		std::array<CD3DX12_ROOT_PARAMETER, 2> rootParameters{};
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);
		//rootParameters[2].InitAsConstantBufferView(2);
		//rootParameters[3].InitAsConstantBufferView(3);
		//rootParameters[4].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

		//std::array<CD3DX12_STATIC_SAMPLER_DESC, 1> samplerDesc{};
		//samplerDesc[0].Init(
		//	0,
		//	D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		//	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		//	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		//	D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		//	0.0f,
		//	1,
		//	D3D12_COMPARISON_FUNC_ALWAYS,
		//	D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
		//	0.0f,
		//	D3D12_FLOAT32_MAX,
		//	D3D12_SHADER_VISIBILITY_PIXEL,
		//	0
		//);

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Init(
			static_cast<UINT>(rootParameters.size()),
			rootParameters.data(),
			0,
			nullptr,
			//static_cast<UINT>(samplerDesc.size()),
			//samplerDesc.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> signature, error;
		if (FAILED(::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
			return false;
		if (FAILED(g_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&g_rootSignature))))
			return false;
		return true;
	}

	static bool CreateCommandList()
	{
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			if (FAILED(g_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i]))))
				return false;
		}
		if (FAILED(g_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[g_frameIndex].Get(), nullptr, IID_PPV_ARGS(&g_commandList))))
			return false;
		if (FAILED(g_commandList->Close()))
			return false;
		return true;
	}

	static bool CreateFence()
	{
		if (FAILED(g_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence))))
			return false;
		g_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!g_fenceEvent)
			return false;
		++g_fenceValues[g_frameIndex];
		return true;
	}

	static bool CreateD3D11On12Device()
	{
		UINT flags{ D3D11_CREATE_DEVICE_BGRA_SUPPORT };
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		ComPtr<ID3D11Device> d3d11Device;
		if (FAILED(::D3D11On12CreateDevice(
			g_d3dDevice.Get(),
			flags,
			nullptr,
			0,
			reinterpret_cast<IUnknown**>(g_commandQueue.GetAddressOf()),
			1,
			0,
			&d3d11Device,
			&g_d3d11DeviceContext,
			nullptr
		)))
			return false;
		if (FAILED(d3d11Device.As(&g_d3d11On12Device)))
			return false;
		return true;
	}

	static bool CreateWrappedResource()
	{
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			D3D11_RESOURCE_FLAGS flags{ D3D11_BIND_RENDER_TARGET };
			if (FAILED(g_d3d11On12Device->CreateWrappedResource(g_renderTargets[i].Get(), &flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&g_wrappedBackBuffers[i]))))
				return false;
		}
		return true;
	}

#ifdef _IMGUI
	static bool InitializeImGui()
	{
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (FAILED(g_d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_imGuiSrvDescHeap))))
			return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiConfigFlags flags{ ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable };
#ifdef _CLIENT
		flags |= ImGuiConfigFlags_ViewportsEnable;
#endif
		ImGui::GetIO().ConfigFlags |= flags;

		if (!::ImGui_ImplWin32_Init(g_hWnd))
			return false;

		if (!::ImGui_ImplDX12_Init(
			g_d3dDevice.Get(),
			FRAME_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			g_imGuiSrvDescHeap.Get(),
			g_imGuiSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_imGuiSrvDescHeap->GetGPUDescriptorHandleForHeapStart()
		))
			return false;

		return true;
	}

	static void CleanUpImGui()
	{
		::ImGui_ImplDX12_Shutdown();
		::ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
#endif

	static bool ResetCommandList()
	{
		if (FAILED(g_commandAllocators[g_frameIndex]->Reset()))
			return false;
		if (FAILED(g_commandList->Reset(g_commandAllocators[g_frameIndex].Get(), nullptr)))
			return false;
		return true;
	}

	static bool ExecuteCommandList()
	{
		if (FAILED(g_commandList->Close()))
			return false;

		ID3D12CommandList* ppCommandList[]{ g_commandList.Get() };
		g_commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
		return true;
	}

	static bool WaitPrevFrame()
	{
		const UINT64 fenceValue{ g_fenceValues[g_frameIndex] };
		if (FAILED(g_commandQueue->Signal(g_fence.Get(), fenceValue)))
			return false;

		if (g_fence->GetCompletedValue() < fenceValue)
		{
			if (FAILED(g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent)))
				return false;
			if (::WaitForSingleObjectEx(g_fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
				return false;
		}

		g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
		g_fenceValues[g_frameIndex] = fenceValue + 1;
		return true;
	}

	static bool WaitForGPU()
	{
		if (FAILED(g_commandQueue->Signal(g_fence.Get(), g_fenceValues[g_frameIndex])))
			return false;
		if (FAILED(g_fence->SetEventOnCompletion(g_fenceValues[g_frameIndex], g_fenceEvent)))
			return false;
		if (::WaitForSingleObjectEx(g_fenceEvent, INFINITE, FALSE) == WAIT_FAILED)
			return false;
		++g_fenceValues[g_frameIndex];
		return true;
	}

	static void OnTextureDestroy(Resource::Texture* texture)
	{

	}

	static std::unique_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<Resource::Model::Vertex>& vertices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, sizeof(Resource::Model::Vertex) * vertices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(vertices.data()), sizeof(Resource::Model::Vertex) * vertices.size() });

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = buffer->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(sizeof(Resource::Model::Vertex) * vertices.size());
		view.StrideInBytes = sizeof(Resource::Model::Vertex);

		return std::make_unique<VertexBuffer>(buffer, view);
	}

	static std::unique_ptr<IndexBuffer> CreateIndexBuffer(const std::vector<int>& indices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, sizeof(int) * indices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(indices.data()), sizeof(int) * indices.size() });

		D3D12_INDEX_BUFFER_VIEW view{};
		view.BufferLocation = buffer->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(sizeof(int) * indices.size());
		view.Format = DXGI_FORMAT_R32_UINT;

		return std::make_unique<IndexBuffer>(buffer, view);
	}

	bool Initialize()
	{
		// D3D12
		if (!CreateDXGIFactory())
			return false;
		if (!CreateD3DDevice())
			return false;
		if (!CreateCommandQueue())
			return false;
		if (!CreateSwapChain())
			return false;
		if (!CreateRtvDsvSrvDescriptorHeap())
			return false;
		if (!CreateRenderTargetView())
			return false;
		if (!CreateDepthStencilView())
			return false;
		if (!CreateRootSignature())
			return false;
		if (!CreateCommandList())
			return false;
		if (!CreateFence())
			return false;

#ifdef _DIRECT2D
		if (!CreateD3D11On12Device())
			return false;
		if (!CreateWrappedResource())
			return false;
#endif

#ifdef _IMGUI
		if (!InitializeImGui())
			return false;
#endif
		return true;
	}

	void CleanUp()
	{
		WaitForGPU();
		::CloseHandle(g_fenceEvent);
#ifdef _IMGUI
		CleanUpImGui();
#endif
	}

	void OnResize(int width, int height)
	{
		WaitPrevFrame();

		g_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		g_scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
		g_renderTargetSize = Int2{ width, height };

		// 렌더 타겟 해제
		for (UINT i = 0; i < FRAME_COUNT; ++i)
		{
			g_renderTargets[i].Reset();
			g_wrappedBackBuffers[i].Reset();
			g_d2dRenderTargets[i].Reset();
			g_fenceValues[i] = g_fenceValues[g_frameIndex];
		}
		g_d2dContext->SetTarget(nullptr);
		g_d2dContext->Flush();
		g_d3d11DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		g_d3d11DeviceContext->Flush();

		// 렌더 타겟 재생성
		DXGI_SWAP_CHAIN_DESC desc{};
		g_swapChain->GetDesc(&desc);
		g_swapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
		g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

		CreateRenderTargetView();
		CreateDepthStencilView();
		CreateWrappedResource();
	}

	DLL_API bool Begin()
	{
		//g_uploadBuffers.clear(); // 이전 프레임에 복사가 완료됐을 것이므로 업로드 버퍼 해제
		g_camera.reset(); // 카메라 초기화

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

		constexpr std::array clearColor{ 0.15625f, 0.171875f, 0.203125f, 1.0f };
		g_commandList->ClearRenderTargetView(rtvHandle, clearColor.data(), 0, nullptr);
		g_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		return true;
	}

	DLL_API bool End()
	{
#ifndef _DIRECT2D
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
#endif
		if (FAILED(g_commandList->Close()))
			return false;

		std::array<ID3D12CommandList*, 1> commandLists{ g_commandList.Get() };
		g_commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		return true;
	}

	DLL_API bool Present()
	{
		if (FAILED(g_swapChain->Present(1, 0)))
			return false;

		WaitPrevFrame();
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

	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary)
	{
		size_t pos{ 0 };
		auto readBytes = [&binary, &pos](auto* dest, size_t size)
			{
				std::memcpy(dest, binary.data() + pos, size);
				pos += size;
			};

		auto read = [&readBytes](auto* dest)
			{
				constexpr auto size{ sizeof(*dest) };
				readBytes(dest, size);
			};

		auto model{ std::make_shared<Resource::Model>() };

		uint32_t meshCount{};
		read(&meshCount);
		model->meshes.resize(meshCount);

		for (auto& mesh : model->meshes)
		{
			uint32_t vertexCount{};
			read(&vertexCount);

			std::vector<Resource::Model::Vertex> vertices(vertexCount);
			for (auto& vertex : vertices)
				read(&vertex);

			uint32_t indexCount{};
			read(&indexCount);

			std::vector<int> indices(indexCount);
			readBytes(indices.data(), sizeof(int) * indexCount);

			auto vertexBuffer{ CreateVertexBuffer(vertices) };
			mesh.vertexBuffer = { vertexBuffer.release(), [](void* pointer) { delete static_cast<VertexBuffer*>(pointer); } };

			auto indexBuffer{ CreateIndexBuffer(indices) };
			mesh.indexBuffer = { indexBuffer.release(), [](void* pointer) { delete static_cast<IndexBuffer*>(pointer); } };

#ifdef _TOOL
			mesh.vertices = std::move(vertices);
			mesh.indices = std::move(indices);
#endif
		}

		assert(pos == binary.size());
		return model;
	}

	std::shared_ptr<Camera> CreateCamera()
	{
		return std::make_shared<Camera>();
	}

	void SetCamera(const std::shared_ptr<Camera>& camera)
	{
		if (g_camera == camera)
			return;

		g_camera = camera;
		g_camera->SetShaderConstants();
	}

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model)
	{
		SetPipelineState(PipelineStateType::Model);
		for (const auto& mesh : model->meshes)
		{
			auto vertexBuffer{ static_cast<VertexBuffer*>(mesh.vertexBuffer.get()) };
			auto indexBuffer{ static_cast<IndexBuffer*>(mesh.indexBuffer.get()) };

			UINT indexCount{};
			switch (indexBuffer->view.Format)
			{
			case DXGI_FORMAT_R16_UINT:
				assert(indexBuffer->view.SizeInBytes % sizeof(uint16_t) == 0);
				indexCount = indexBuffer->view.SizeInBytes / sizeof(uint16_t);
				break;
			case DXGI_FORMAT_R32_UINT:
				assert(indexBuffer->view.SizeInBytes % sizeof(uint32_t) == 0);
				indexCount = indexBuffer->view.SizeInBytes / sizeof(uint32_t);
				break;
			default:
				assert(false && "INVALID INDEX BUFFER FORMAT");
				continue;
			}

			g_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_commandList->IASetVertexBuffers(0, 1, &vertexBuffer->view);
			g_commandList->IASetIndexBuffer(&indexBuffer->view);
			g_commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		}
	}

	void CreateResourceFromTexture(const std::shared_ptr<Resource::Texture>& texture)
	{
	}
}
