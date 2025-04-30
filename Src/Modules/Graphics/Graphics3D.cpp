#include "Stdafx.h"
#include "Camera.h"
#include "Descriptor.h"
#include "DescriptorManager.h"
#include "Global.h"
#include "Graphics3D.h"
#include "Graphics3DUtil.h"
#include "PipelineState.h"
#include "RenderTarget.h"
#include "SwapChain.h"
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

		DescriptorManager::Instantiate();
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
		if (!::GetClientRect(g_hWnd, &rect))
			return false;

		int width{ rect.right - rect.left };
		int height{ rect.bottom - rect.top };
		g_swapChain = std::make_unique<SwapChain>(width, height);
		g_renderTargetSize = Int2{ width, height };
		return true;
	}

	static bool CreateRootSignature()
	{
		std::array<CD3DX12_ROOT_PARAMETER, 2> rootParameters{};
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.Init(
			static_cast<UINT>(rootParameters.size()),
			rootParameters.data(),
			0,
			nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		);

		ComPtr<ID3DBlob> signature, error;
		if (FAILED(::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
			return false;
		if (FAILED(g_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&g_rootSignature))))
			return false;
		return true;
	}

#ifdef _DIRECT2D
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

	static bool CreateD2DFactory()
	{
		if (FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2dFactory.GetAddressOf())))
			return false;
		return true;
	}

	static bool CreateD2DDevice()
	{
		ComPtr<IDXGIDevice> dxgiDevice;
		if (FAILED(g_d3d11On12Device.As(&dxgiDevice)))
			return false;
		if (FAILED(g_d2dFactory->CreateDevice(dxgiDevice.Get(), &g_d2dDevice)))
			return false;
		if (FAILED(g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_d2dContext)))
			return false;
		return true;
	}
#endif

#ifdef _IMGUI
	static bool InitializeImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiConfigFlags flags{ ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable };
#ifdef _CLIENT
		flags |= ImGuiConfigFlags_ViewportsEnable;
#endif
		ImGui::GetIO().ConfigFlags |= flags;

		if (!::ImGui_ImplWin32_Init(g_hWnd))
			return false;

		auto dm{ DescriptorManager::GetInstance() };
		if (!dm)
			return false;

		if (!::ImGui_ImplDX12_Init(
			g_d3dDevice.Get(),
			SwapChain::FRAME_COUNT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			dm->GetImGuiSrvHeap().Get(),
			dm->GetImGuiSrvCpuHandle(),
			dm->GetImGuiSrvGpuHandle()
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

	static void OnTextureDestroy(Resource::Texture* texture)
	{

	}

	static std::unique_ptr<VertexBuffer> CreateVertexBuffer(const std::vector<Resource::Model::Vertex>& vertices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, sizeof(Resource::Model::Vertex) * vertices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(vertices.data()), sizeof(Resource::Model::Vertex) * vertices.size() });
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));

		D3D12_VERTEX_BUFFER_VIEW view{};
		view.BufferLocation = buffer->GetGPUVirtualAddress();
		view.SizeInBytes = static_cast<UINT>(sizeof(Resource::Model::Vertex) * vertices.size());
		view.StrideInBytes = sizeof(Resource::Model::Vertex);

		return std::make_unique<VertexBuffer>(buffer, view);
	}

	static std::unique_ptr<IndexBuffer> CreateIndexBuffer(const std::vector<int>& indices)
	{
		auto buffer{ CreateResourceBuffer(D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, sizeof(int) * indices.size()) };
		CopyResource(buffer, std::span{ reinterpret_cast<const std::byte*>(indices.data()), sizeof(int) * indices.size() });
		g_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER));

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
#ifdef _DIRECT2D
		if (!CreateD3D11On12Device())
			return false;
		if (!CreateD2DFactory())
			return false;
		if (!CreateD2DDevice())
			return false;
#endif
		if (!CreateSwapChain())
			return false;
		if (!CreateRootSignature())
			return false;

#ifdef _IMGUI
		if (!InitializeImGui())
			return false;
#endif
		return true;
	}

	void CleanUp()
	{
		g_swapChain.reset();
#ifdef _IMGUI
		CleanUpImGui();
#endif
	}

	DLL_API void Begin()
	{
		//g_uploadBuffers.clear(); // 이전 프레임에 복사가 완료됐을 것이므로 업로드 버퍼 해제
		g_camera.reset(); // 카메라 초기화
		g_swapChain->Begin3D();
	}

	DLL_API void End()
	{
		g_swapChain->End3D();
	}

	DLL_API void Present()
	{
		g_swapChain->Present();
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

	std::shared_ptr<RenderTarget> CreateRenderTarget(UINT width, UINT height)
	{
		return std::make_shared<RenderTarget>(width, height);
	}

	DLL_API void PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget)
	{
		g_swapChain->PushRenderTarget(renderTarget);
	}

	DLL_API void PopRenderTarget()
	{
		g_swapChain->PopRenderTarget();
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
