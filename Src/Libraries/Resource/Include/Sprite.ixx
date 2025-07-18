module;

#include <Unknwn.h>
#include <wrl.h>

export module rewriter.library.resource:sprite;

import std;
import rewriter.common.type;

using Microsoft::WRL::ComPtr;

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

export namespace Resource
{
	struct Sprite;

	RESOURCE_API std::shared_ptr<Sprite> NewSprite(IUnknown* bitmap, Float2 size);
	RESOURCE_API IUnknown* GetSpriteBitmap(const std::shared_ptr<Sprite>& sprite);
	RESOURCE_API Float2 GetSpriteSize(const std::shared_ptr<Sprite>& sprite);

#ifdef _TOOL
	RESOURCE_API void SetSpriteBinary(const std::shared_ptr<Sprite>& sprite, std::span<std::byte> binary);
#endif
}
