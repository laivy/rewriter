#pragma once

namespace Resource
{
	struct Model;
}

namespace Graphics::D3D
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
