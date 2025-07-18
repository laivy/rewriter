module;

#include <Unknwn.h>
#include <wrl.h>

module rewriter.library.resource:sprite;

using Microsoft::WRL::ComPtr;

namespace Resource
{
	struct Sprite
	{
		ComPtr<IUnknown> bitmap; // ID2D1Bitmap
		Float2 size;

#ifdef _TOOL
		std::vector<std::byte> binary;
#endif
	};

	std::shared_ptr<Sprite> NewSprite(IUnknown* bitmap, Float2 size)
	{
		auto sprite{ std::make_shared<Sprite>() };
		sprite->bitmap = bitmap;
		sprite->size = size;
		return sprite;
	}

	IUnknown* GetSpriteBitmap(const std::shared_ptr<Sprite>& sprite)
	{
		return sprite->bitmap.Get();
	}

	Float2 GetSpriteSize(const std::shared_ptr<Sprite>& sprite)
	{
		return sprite->size;
	}

#ifdef _TOOL
	void SetSpriteBinary(const std::shared_ptr<Sprite>& sprite, std::span<std::byte> binary)
	{
		sprite->binary.reserve(binary.size());
		std::ranges::copy(binary, std::back_inserter(sprite->binary));
	}
#endif
}
