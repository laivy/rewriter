#pragma once

struct IUnknown;

namespace Resource
{
	class Texture
	{
#if defined _CLIENT || defined _TOOL
	public:
		DLL_API Texture(std::span<std::byte> binary);
		~Texture() = default;

	private:
		ComPtr<IUnknown> m_resource;
#endif
	};
}
