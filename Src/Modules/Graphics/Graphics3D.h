#pragma once

namespace Resource
{
	struct Model;
	class Texture;
}

namespace Graphics::D3D
{
	DLL_API bool Begin();
	DLL_API bool End();

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
	DLL_API std::shared_ptr<Resource::Model> LoadModel(std::span<std::byte> binary);

	DLL_API void Render(const std::shared_ptr<Resource::Model>& model);
}
