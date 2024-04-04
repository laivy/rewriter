#include "Stdafx.h"
#include "Global.h"

namespace Global
{
	Event<std::shared_ptr<Resource::Property>> OnPropertyAdd;
	Event<std::shared_ptr<Resource::Property>> OnPropertyDelete;
	Event<std::shared_ptr<Resource::Property>> OnPropertySelect;
}