#pragma once

struct ID2D1Bitmap;
struct ID2D1DeviceContext2;
struct ID3D12Resource;

namespace Resource
{
	class Image
	{
	public:
		enum class Type
		{
			D2D, D3D
		};

	public:
		Image();
		~Image();

		void SetBuffer(std::byte* buffer, size_t size);
		void SetD2DBitmap(ID2D1Bitmap* bitmap);

		std::span<std::byte> GetBuffer() const;

		__declspec(dllexport) ID2D1Bitmap* GetD2DBitmap() const;
		__declspec(dllexport) INT2 GetSize() const;
		__declspec(dllexport) void UseAs(const ComPtr<ID2D1DeviceContext2>& ctx, Type type);

	private:
		std::unique_ptr<std::byte[]> m_buffer;
		size_t m_bufferSize;
		ComPtr<ID2D1Bitmap> m_d2dBitmap;
		ComPtr<ID3D12Resource> m_d3dResource;
	};
}
