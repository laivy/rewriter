#include "Pch.h"
#include "Context.h"
#include "Graphics.h"

using Microsoft::WRL::ComPtr;

namespace
{
	bool CreateDXGIFactory()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return false;

		UINT flags{ 0 };
#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		if (FAILED(::CreateDXGIFactory2(flags, IID_PPV_ARGS(&ctx->dxgiFactory))))
			return false;
		return true;
	}

	bool CreateD3D12Device()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return false;

		ComPtr<IDXGIAdapter1> adapter;
		for (UINT i{ 0 }; ctx->dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 desc{};
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;
			if (FAILED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&ctx->d3d12Device))))
				continue;
			break;
		}
		if (!ctx->d3d12Device)
		{
			ctx->dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			if (FAILED(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&ctx->d3d12Device))))
				return false;
		}
		return true;
	}

	bool CreateCommandQueue()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device)
			return false;

		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(ctx->d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&ctx->commandQueue))))
			return false;
		return true;
	}

	bool CreateD3D11On12Device()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device)
			return false;

		UINT flags{ D3D11_CREATE_DEVICE_BGRA_SUPPORT };
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		ComPtr<ID3D11Device> d3d11Device;
		if (FAILED(::D3D11On12CreateDevice(
			ctx->d3d12Device.Get(),
			flags,
			nullptr,
			0,
			reinterpret_cast<IUnknown**>(ctx->commandQueue.GetAddressOf()),
			1,
			0,
			&d3d11Device,
			&ctx->d3d11DeviceContext,
			nullptr
		)))
			return false;
		if (FAILED(d3d11Device.As(&ctx->d3d11On12Device)))
			return false;
		return true;
	}

	bool InitializeDirect2D()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return false;
		if (FAILED(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, ctx->d2dFactory.GetAddressOf())))
			return false;

		ComPtr<IDXGIDevice> dxgiDevice;
		if (FAILED(ctx->d3d11On12Device.As(&dxgiDevice)))
			return false;
		if (FAILED(ctx->d2dFactory->CreateDevice(dxgiDevice.Get(), &ctx->d2dDevice)))
			return false;
		if (FAILED(ctx->d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ctx->d2dContext)))
			return false;
		return true;
	}

#ifdef _IMGUI
	bool InitializeImGui()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		if (!::ImGui_ImplWin32_Init(ctx->hWnd))
			return false;

		ImGui_ImplDX12_InitInfo info{};
		info.Device = ctx->d3d12Device.Get();
		info.CommandQueue = ctx->commandQueue.Get();
		info.NumFramesInFlight = 2;
		info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		info.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		info.SrvDescriptorHeap = ctx->descriptorManager->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
		{
			auto ctx{ Graphics::Context::GetInstance() };
			if (!ctx)
				return;

			auto desc{ ctx->descriptorManager->Allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };
			*out_cpu_handle = desc.cpuHandle;
			*out_gpu_handle = desc.gpuHandle;
		};
		info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
		{
			auto ctx{ Graphics::Context::GetInstance() };
			if (!ctx)
				return;

			Graphics::Descriptor::Handle handle{};
			handle.type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			handle.cpuHandle = cpu_handle;
			handle.gpuHandle = gpu_handle;
			ctx->descriptorManager->Free(handle);
		};
		if (!::ImGui_ImplDX12_Init(&info))
			return false;

		return true;
	}

	void CleanUpImGui()
	{
		::ImGui_ImplDX12_Shutdown();
		::ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
#endif
}

namespace Graphics
{
	bool Initialize(void* hWnd)
	{
		auto ctx{ Context::Instantiate() };
		ctx->hWnd = hWnd;
		if (!CreateDXGIFactory())
			return false;
		if (!CreateD3D12Device())
			return false;
		ctx->descriptorManager = std::make_unique<Descriptor::Manager>();
		if (!ctx->descriptorManager)
			return false;
		if (!CreateCommandQueue())
			return false;
		if (!CreateD3D11On12Device())
			return false;
		if (!InitializeDirect2D())
			return false;
		return true;
	}

	void Finalize()
	{
		Context::Destroy();
	}

	Resource::Sprite LoadSprite(std::span<char> binary)
	{
		return {};
	}

	bool Begin()
	{
		return true;
	}

	bool End()
	{
		return true;
	}

	bool Present()
	{
		return true;
	}
}
