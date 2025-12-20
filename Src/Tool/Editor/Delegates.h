#pragma once

namespace Delegates
{
	extern Delegate<Int2> OnWindowResized;
	extern Delegate<Resource::ID> OnPropertyAdded;
	extern Delegate<Resource::ID> OnPropertyDeleted;
	extern Delegate<Resource::ID> OnPropertyModified;
	extern Delegate<Resource::ID> OnPropertySelected;
}
