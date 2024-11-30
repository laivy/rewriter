#include "Stdafx.h"
#include "Texture.h"

namespace Resource
{
	DLL_API Texture::Texture(const ComPtr<IUnknown>& resource, const std::function<void(Texture*)> onDestroy) :
		m_resource{ resource },
		m_callback{ onDestroy }
	{
	}

	DLL_API IUnknown* Texture::Get() const
	{
		return m_resource.Get();
	}
}
