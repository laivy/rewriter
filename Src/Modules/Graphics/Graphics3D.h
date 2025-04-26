#pragma once

namespace Resource
{
	struct Model;
	class Texture;
}

namespace Graphics::D3D
{
	class Camera;

	bool Initialize();
	void CleanUp();

	DLL_API void Begin();
	DLL_API void End();
	DLL_API void Present();

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary);

	DLL_API std::shared_ptr<Camera> CreateCamera();
	DLL_API void SetCamera(const std::shared_ptr<Camera>& camera);

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model);
}
