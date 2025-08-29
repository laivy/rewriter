#include "Stdafx.h"
#include "Delegates.h"

namespace Delegates
{
	Delegate<int, int> OnWindowResized;
	Delegate<Resource::ID> OnPropertyAdded;
	Delegate<Resource::ID> OnPropertyDeleted;
	Delegate<Resource::ID> OnPropertyModified;
	Delegate<Resource::ID> OnPropertySelected;
}
