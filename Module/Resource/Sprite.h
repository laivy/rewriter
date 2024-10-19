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
		DLL_API INT2 GetSize() const;

#ifdef _TOOL
		DLL_API uint32_t GetBinarySize() const;
		DLL_API std::byte* GetBinary() const;
#endif

	private:
		ComPtr<ID2D1Bitmap> m_bitmap;

#ifdef _TOOL
		uint32_t m_binarySize;
		std::shared_ptr<std::byte[]> m_binary;
#endif
#endif
	};
}
