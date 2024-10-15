#pragma once

#if defined _CLIENT || defined _TOOL
namespace Resource
{
	class PNG
	{
	public:
		DLL_API PNG(std::byte* binary, uint32_t size);
		~PNG() = default;

		DLL_API ID2D1Bitmap* Get() const;
		DLL_API INT2 GetSize() const;

	private:
		ComPtr<ID2D1Bitmap> m_bitmap;

#ifdef _TOOL
	public:
		DLL_API uint32_t GetBinarySize() const;
		DLL_API std::byte* GetBinary() const;

	private:
		uint32_t m_binarySize;
		std::unique_ptr<std::byte[]> m_binary;
#endif
	};
}
#endif
