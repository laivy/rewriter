#pragma once

namespace Resource
{
	class Texture;
}

namespace Graphics::D3D
{
	DLL_API bool Begin();
	DLL_API bool End();

	DLL_API std::shared_ptr<Resource::Texture> LoadTexture(std::span<std::byte> binary);
}
