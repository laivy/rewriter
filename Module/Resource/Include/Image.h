#pragma once

struct ID2D1DeviceContext2;

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
		__declspec(dllexport) Image();
		~Image() = default;

		__declspec(dllexport) void SetBuffer(std::byte* buffer, size_t size);

		std::span<std::byte> GetBuffer() const;

		__declspec(dllexport) IUnknown* Get() const;
		__declspec(dllexport) INT2 GetSize() const;
		__declspec(dllexport) void Use(const ComPtr<ID2D1DeviceContext2>& ctx);

	private:
		Type m_type;
		std::unique_ptr<std::byte[]> m_buffer;
		size_t m_bufferSize;
		ComPtr<IUnknown> m_resource;
	};
}
