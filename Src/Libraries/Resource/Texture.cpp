module Resource:Texture;

import std;

#include <wrl.h>
using Microsoft::WRL::ComPtr;

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
