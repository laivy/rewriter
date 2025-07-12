export module Library.Graphics.D3D;

export import Library.Graphics;
export import :Camera;
export import :ConstantBuffer;
export import :Descriptor;
export import :DescriptorManager;
export import :PipelineState;
export import :RenderTarget;
export import :SwapChain;
#ifdef _IMGUI
export import :ImGui;
#endif

import std;
import Library.Resource;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics::D3D
{
	class RenderTarget;

	bool Initialize();
	void CleanUp();

	GRAPHICS_API void Begin();
	GRAPHICS_API void End();
	GRAPHICS_API void Present();

	/*
	GRAPHICS_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
	*/
	GRAPHICS_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary);

	GRAPHICS_API std::shared_ptr<RenderTarget> CreateRenderTarget(std::uint32_t width, std::uint32_t height);
	GRAPHICS_API void PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget);
	GRAPHICS_API void PopRenderTarget();

	GRAPHICS_API void Render(const std::shared_ptr<Resource::Model>& model);
}
