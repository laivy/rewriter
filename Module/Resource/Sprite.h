#pragma once

namespace Resource
{
	class Sprite
	{
#if defined _CLIENT || defined _TOOL
	public:
		DLL_API Sprite();
		DLL_API Sprite(std::span<std::byte> binary);
		~Sprite() = default;

		DLL_API ID2D1Bitmap* Get() const;
		DLL_API FLOAT2 GetSize() const;

#ifdef _TOOL
		DLL_API std::span<const std::byte> GetBinary() const;
#endif

	private:
		ComPtr<ID2D1Bitmap> m_bitmap;
		FLOAT2 m_size;

#ifdef _TOOL
		std::vector<std::byte> m_binary;
#endif
#endif
	};
}
