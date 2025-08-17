#include "Stdafx.h"
#include "Delegates.h"

namespace Delegates
{
	Delegate<int, int> OnWindowResized;
	Delegate<Resource::Property::ID> OnPropertyAdded;
	Delegate<Resource::Property::ID> OnPropertyDeleted;
	Delegate<Resource::Property::ID> OnPropertyModified;
	Delegate<Resource::Property::ID> OnPropertySelected;
}
