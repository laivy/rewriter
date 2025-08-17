#pragma once
#include "Sprite.h"
#include "Resource.h"

extern Delegate<const Resource::Initializer&> OnInitialize;
extern Delegate<> OnUninitialize;
