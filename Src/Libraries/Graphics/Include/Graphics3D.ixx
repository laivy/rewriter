export module rewriter.library.graphics.direct3d;

export import rewriter.library.graphics;
export import :camera;
export import :constant_buffer;
export import :descriptor;
export import :descriptor_manager;
export import :pipeline_state;
export import :render_target;
export import :swap_chain;
#ifdef _IMGUI
export import :imgui;
#endif

import std;
import rewriter.library.resource;

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
