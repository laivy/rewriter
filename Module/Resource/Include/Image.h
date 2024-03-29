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
			NONE, D2D, D3D
		};

	public:
		Image();
		~Image();

		void SetBuffer(std::byte* buffer, size_t size);

		std::span<std::byte> GetBuffer() const;

		__declspec(dllexport) IUnknown* Get() const;
		__declspec(dllexport) INT2 GetSize() const;
		__declspec(dllexport) void UseAs(const ComPtr<ID2D1DeviceContext2>& ctx, Type type);

	private:
		Type m_type;
		std::unique_ptr<std::byte[]> m_buffer;
		size_t m_bufferSize;
		ComPtr<IUnknown> m_resource;
	};
}
