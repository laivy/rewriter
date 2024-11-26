#pragma once

namespace Resource
{
	class Texture
	{
#if defined _CLIENT || defined _TOOL
	public:
		DLL_API Texture(std::span<std::byte> binary);
		~Texture() = default;

	private:
		ComPtr<ID3D12Resource> m_resource;
#endif
	};
}
