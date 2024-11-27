#pragma once

struct IUnknown;

namespace Resource
{
	class Sprite
	{
	public:
		DLL_API Sprite();
		~Sprite() = default;

		DLL_API void Set(const ComPtr<IUnknown>& bitmap, const Float2& size);
		DLL_API IUnknown* Get() const;
		DLL_API Float2 GetSize() const;

	private:
		ComPtr<IUnknown> m_bitmap;
		Float2 m_size;

#ifdef _TOOL
	public:
		DLL_API void SetBinary(std::span<std::byte> binary);
		DLL_API std::span<const std::byte> GetBinary() const;

	private:
		std::vector<std::byte> m_binary;
#endif
	};
}
