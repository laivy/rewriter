#include "Stdafx.h"
#include "Delegates.h"

namespace Delegates
{
	Delegate<const Resource::Initializer&> OnInitialize;
	Delegate<> OnUninitialize;
}
