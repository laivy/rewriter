#include "Stdafx.h"
#include "Texture.h"

namespace Resource
{
	Texture::Texture(const ComPtr<IUnknown>& resource) :
		m_resource{ resource }
	{
	}

	IUnknown* Texture::Get() const
	{
		return m_resource.Get();
	}
}
