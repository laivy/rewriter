#pragma once
#include "Sprite.h"
#include "Resource.h"

namespace Delegates
{
	extern Delegate<const Resource::Initializer&> OnInitialize;
	extern Delegate<> OnUninitialize;
}
