#pragma once
#if defined _CLIENT || defined _TOOL

namespace Resource
{
	class PNG
	{
	public:
		DLL_API PNG(std::byte* binary, size_t size);
		~PNG() = default;

		DLL_API ID2D1Bitmap* Get() const;
		DLL_API INT2 GetSize() const;

	private:
		ComPtr<ID2D1Bitmap> m_bitmap;

#ifdef _TOOL
	public:
		DLL_API std::byte* GetBinary() const;
		DLL_API size_t GetBinarySize() const;

	private:
		std::unique_ptr<std::byte[]> m_binary;
		size_t m_binarySize;
#endif // _TOOL
	};
}
#endif // defined _CLIENT || defined _TOOL