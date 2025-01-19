#pragma once

struct IUnknown;

namespace Resource
{
	class Sprite
	{
	public:
		DLL_API Sprite(const ComPtr<IUnknown>& bitmap, const Float2& size);
		~Sprite() = default;

		DLL_API IUnknown* Get() const;
		DLL_API Float2 GetSize() const;

	private:
		ComPtr<IUnknown> m_bitmap;
		Float2 m_size;

#ifdef _TOOL
	public:
		DLL_API void SetBinary(std::span<std::byte> binary);
		DLL_API std::span<std::byte> GetBinary();

	private:
		std::vector<std::byte> m_binary;
#endif
	};
}
