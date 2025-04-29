#pragma once

namespace Resource
{
	struct Model;
	class Texture;
}

namespace Graphics::D3D
{
	class Camera;
	class RenderTarget;

	bool Initialize();
	void CleanUp();

	DLL_API void Begin();
	DLL_API void End();
	DLL_API void Present();

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary);

	DLL_API std::shared_ptr<Camera> CreateCamera();
	DLL_API void SetCamera(const std::shared_ptr<Camera>& camera);

	DLL_API std::shared_ptr<RenderTarget> CreateRenderTarget(UINT width, UINT height);
	DLL_API void PushRenderTarget(const std::shared_ptr<RenderTarget>& renderTarget);
	DLL_API void PopRenderTarget();

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model);
}
