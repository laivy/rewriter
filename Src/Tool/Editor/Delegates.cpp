#include "Pch.h"
#include "Delegates.h"

namespace Delegates
{
	Delegate<Int2> OnWindowResized;
	Delegate<Resource::ID> OnPropertyAdded;
	Delegate<Resource::ID> OnPropertyDeleted;
	Delegate<Resource::ID> OnPropertyModified;
	Delegate<Resource::ID> OnPropertySelected;
}
