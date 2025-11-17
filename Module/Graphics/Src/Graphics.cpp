#include "Pch.h"
#include "Context.h"
#include "Descriptor.h"
#include "Graphics.h"
#include "SwapChain.h"
#ifdef _IMGUI
#include "../Include/Graphics/ImGui.h"
#endif

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

	bool CreateCommandAllocator()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device)
			return false;

		if (FAILED(ctx->d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&ctx->commandAllocator))))
			return false;
		return true;
	}

	bool CreateCommandList()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device || !ctx->commandAllocator)
			return false;

		if (FAILED(ctx->d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ctx->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&ctx->commandList))))
			return false;
		if (FAILED(ctx->commandList->Close()))
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
		{
			return false;
		}
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

	bool CreateFence()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->d3d12Device)
			return false;
		if (FAILED(ctx->d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&ctx->fence))))
			return false;
		ctx->fenceValue = 1;
		ctx->fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!ctx->fenceEvent)
			return false;
		return true;
	}

#ifdef _IMGUI
	bool InitializeImGui()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx)
			return false;

		// ImGui 전용 D3D12 리소스 생성
		if (auto imguiCtx{ Graphics::ImGui::Context::Instantiate() })
		{
			if (FAILED(ctx->d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&imguiCtx->commandAllocator))))
				return false;
			if (FAILED(ctx->d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, imguiCtx->commandAllocator.Get(), nullptr, IID_PPV_ARGS(&imguiCtx->commandList))))
				return false;
			imguiCtx->commandList->Close();
			if (FAILED(ctx->d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&imguiCtx->fence))))
				return false;
			imguiCtx->fenceValue = 1;
			imguiCtx->fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (!imguiCtx->fenceEvent)
				return false;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		if (!::ImGui_ImplWin32_Init(ctx->hWnd))
			return false;

		ImGui_ImplDX12_InitInfo info{};
		info.Device = ctx->d3d12Device.Get();
		info.CommandQueue = ctx->commandQueue.Get();
		info.NumFramesInFlight = Graphics::Context::FrameCount;
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
#endif

	bool WaitForPreviousFrame()
	{
		auto ctx{ Graphics::Context::GetInstance() };
		if (!ctx || !ctx->commandQueue || !ctx->fence || !ctx->fenceEvent)
			return false;

		const UINT64 fenceValue{ ctx->fenceValue };
		if (FAILED(ctx->commandQueue->Signal(ctx->fence.Get(), fenceValue)))
			return false;

		if (ctx->fence->GetCompletedValue() < fenceValue)
		{
			if (FAILED(ctx->fence->SetEventOnCompletion(fenceValue, ctx->fenceEvent)))
				return false;
			if (::WaitForSingleObject(ctx->fenceEvent, INFINITE) == WAIT_FAILED)
				return false;
		}
		++ctx->fenceValue;
		return true;
	}
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
		if (!CreateCommandQueue())
			return false;
		if (!CreateCommandAllocator())
			return false;
		if (!CreateCommandList())
			return false;
		if (!CreateD3D11On12Device())
			return false;
		if (!InitializeDirect2D())
			return false;
		if (!CreateFence())
			return false;
		ctx->descriptorManager = std::make_unique<Descriptor::Manager>();
		if (!ctx->descriptorManager)
			return false;
		ctx->swapChain = std::make_unique<SwapChain>();
		if (!ctx->swapChain)
			return false;
#ifdef _IMGUI
		if (!InitializeImGui())
			return false;
#endif
		return true;
	}

	void Finalize()
	{
#ifdef _IMGUI
		::ImGui_ImplDX12_Shutdown();
		::ImGui_ImplWin32_Shutdown();
		::ImGui::DestroyContext();
		ImGui::Context::Destroy();
#endif
		Context::Destroy();
	}

	Resource::Sprite LoadSprite(Resource::ID id, std::span<char> binary)
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx || !ctx->d2dContext)
			return {};

		ComPtr<IWICImagingFactory> factory;
		ComPtr<IWICStream> stream;
		ComPtr<IWICBitmapDecoder> decoder;
		ComPtr<IWICFormatConverter> converter;
		ComPtr<IWICBitmapFrameDecode> frameDecode;
		ComPtr<ID2D1Bitmap> bitmap;
		if (FAILED(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
			return {};
		if (FAILED(factory->CreateStream(&stream)))
			return {};
		if (FAILED(stream->InitializeFromMemory(reinterpret_cast<WICInProcPointer>(binary.data()), static_cast<DWORD>(binary.size()))))
			return {};
		if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder)))
			return {};
		if (FAILED(factory->CreateFormatConverter(&converter)))
			return {};
		if (FAILED(decoder->GetFrame(0, &frameDecode)))
			return {};
		if (FAILED(converter->Initialize(frameDecode.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut)))
			return {};
		if (FAILED(ctx->d2dContext->CreateBitmapFromWicBitmap(converter.Get(), bitmap.GetAddressOf())))
			return {};

		const D2D1_SIZE_F size{ bitmap->GetSize() };
		Resource::Sprite sprite{};
#ifdef _TOOL
		sprite.binary = std::make_shared<std::vector<char>>(binary.begin(), binary.end());
#endif
		sprite.width = static_cast<std::uint32_t>(size.width);
		sprite.height = static_cast<std::uint32_t>(size.height);
#ifdef _IMGUI
		if (auto path{ Resource::GetPath(id) })
			ImGui::CreateTexture(*path, binary);
#endif
		return sprite;
	}

	bool Begin3D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;

		if (FAILED(ctx->commandAllocator->Reset()))
			return false;
		if (FAILED(ctx->commandList->Reset(ctx->commandAllocator.Get(), nullptr)))
			return false;
		if (!ctx->swapChain->Bind3D(ctx->commandList.Get()))
			return false;

		const std::array<ID3D12DescriptorHeap*, 2> descriptorHeaps{
			ctx->descriptorManager->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			ctx->descriptorManager->GetHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		};
		ctx->commandList->SetDescriptorHeaps(static_cast<UINT>(descriptorHeaps.size()), descriptorHeaps.data());
		return true;
	}

	bool End3D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;
		if (FAILED(ctx->commandList->Close()))
			return false;

		const std::array<ID3D12CommandList*, 1> commandLists{ ctx->commandList.Get() };
		ctx->commandQueue->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());
		return true;
	}

	bool Begin2D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;
		if (!ctx->swapChain->Bind2D())
			return false;
		return true;
	}

	bool End2D()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;
		if (!ctx->swapChain->Unbind2D())
			return false;
		return true;
	}

	bool Present()
	{
		auto ctx{ Context::GetInstance() };
		if (!ctx)
			return false;
		if (!ctx->swapChain->Present())
			return false;
		if (!WaitForPreviousFrame())
			return false;
		return true;
	}
}
