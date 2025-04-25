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

	void OnResize(int width, int height);

	DLL_API bool Begin();
	DLL_API bool End();
	DLL_API bool Present();

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary);

	DLL_API std::shared_ptr<Camera> CreateCamera();
	DLL_API void SetCamera(const std::shared_ptr<Camera>& camera);

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model);
}
