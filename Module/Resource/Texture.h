#pragma once

struct IUnknown;

namespace Resource
{
	class Texture
	{
	public:
		DLL_API Texture(const ComPtr<IUnknown>& resource, const std::function<void(Texture*)> onDestroy);
		~Texture() = default;

		DLL_API IUnknown* Get() const;

	private:
		ComPtr<IUnknown> m_resource;
		std::function<void(Texture*)> m_callback;
	};
}
