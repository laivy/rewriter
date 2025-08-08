#pragma once

#ifdef _RESOURCE
#define RESOURCE_API __declspec(dllexport)
#else
#define RESOURCE_API __declspec(dllimport)
#endif

#include "../Model.h"
#include "../Property.h"
#include "../Resource.h"
#include "../Sprite.h"
