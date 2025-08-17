#pragma once

namespace Delegates
{
	extern Delegate<int, int> OnWindowResized;
	extern Delegate<Resource::Property::ID> OnPropertyAdded;
	extern Delegate<Resource::Property::ID> OnPropertyDeleted;
	extern Delegate<Resource::Property::ID> OnPropertyModified;
	extern Delegate<Resource::Property::ID> OnPropertySelected;
}
