#pragma once
#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API __declspec(dllimport)

#include "../Model.h"
#include "../Property.h"
#include "../Resource.h"
#include "../Sprite.h"
#include "../Texture.h"
