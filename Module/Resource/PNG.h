#pragma once

struct ID2D1DeviceContext2;

namespace Resource
{
	class PNG
	{
	public:
		__declspec(dllexport) PNG(std::byte* binary, size_t size);
		~PNG() = default;

		__declspec(dllexport) ID2D1Bitmap* Get() const;
		__declspec(dllexport) INT2 GetSize() const;

	private:
		ComPtr<ID2D1Bitmap> m_bitmap;

#ifdef _TOOL
	public:
		std::byte* GetBinary() const;
		size_t GetBinarySize() const;

	private:
		std::unique_ptr<std::byte[]> m_binary;
		size_t m_binarySize;
#endif
	};
}
