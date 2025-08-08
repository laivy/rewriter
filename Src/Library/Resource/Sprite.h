#pragma once

struct IUnknown;

namespace Resource
{
	struct Sprite;

	RESOURCE_API std::shared_ptr<Sprite> NewSprite(IUnknown* bitmap, Float2 size);
	RESOURCE_API IUnknown* GetSpriteBitmap(const std::shared_ptr<Sprite>& sprite);
	RESOURCE_API Float2 GetSpriteSize(const std::shared_ptr<Sprite>& sprite);

#ifdef _TOOL
	RESOURCE_API void SetSpriteBinary(const std::shared_ptr<Sprite>& sprite, std::span<std::byte> binary);
#endif
}
