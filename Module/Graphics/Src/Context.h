#pragma once

namespace Graphics
{
	namespace Descriptor
	{
		class Manager;
	}

	class SwapChain;

	struct Context : Singleton<Context>
	{
		static constexpr UINT FrameCount{ 2 };

		void* hWnd;
		Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> d3d12Device;
		Microsoft::WRL::ComPtr<ID3D11On12Device> d3d11On12Device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
		Microsoft::WRL::ComPtr<ID2D1Factory3> d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device2> d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext2> d2dContext;
		Microsoft::WRL::ComPtr<IDWriteFactory5> dwriteFactory;
		std::unique_ptr<Descriptor::Manager> descriptorManager;
		std::unique_ptr<SwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		UINT64 fenceValue;
		HANDLE fenceEvent;
	};

#ifdef _IMGUI
	namespace ImGui
	{
		struct Texture
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			ImTextureID id;
			std::uint32_t width;
			std::uint32_t height;
		};

		struct Context : Singleton<Context>
		{
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
			Microsoft::WRL::ComPtr<ID3D12Fence> fence;
			UINT64 fenceValue;
			HANDLE fenceEvent;

			std::unordered_map<Resource::ID, Texture> textures;
			std::vector<std::pair<Resource::ID, Texture>> textureHolder;
		};
	}
#endif
}
