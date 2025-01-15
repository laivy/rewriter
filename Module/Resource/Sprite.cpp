#include "Stdafx.h"
#include "Resource.h"
#include "Sprite.h"

namespace Resource
{
	DLL_API Sprite::Sprite(const ComPtr<IUnknown>& bitmap, const Float2& size) :
		m_bitmap{ bitmap },
		m_size{ size }
	{
	}

	DLL_API IUnknown* Sprite::Get() const
	{
		return m_bitmap.Get();
	}

	DLL_API Float2 Sprite::GetSize() const
	{
		return m_size;
	}

#ifdef _TOOL
	DLL_API void Sprite::SetBinary(std::span<std::byte> binary)
	{
		m_binary.reserve(binary.size());
		std::ranges::copy(binary, std::back_inserter(m_binary));
	}

	DLL_API std::span<const std::byte> Sprite::GetBinary() const
	{
		return std::span{ m_binary.data(), m_binary.size() };
	}
#endif
}
