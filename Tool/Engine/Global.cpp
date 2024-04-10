#include "Stdafx.h"
#include "Global.h"
#include "PropInfo.h"

namespace Global
{
	Event<std::shared_ptr<Resource::Property>> OnPropertyAdd;
	Event<std::shared_ptr<Resource::Property>> OnPropertyDelete;
	Event<std::shared_ptr<Resource::Property>> OnPropertySelect;

	std::vector<std::shared_ptr<Resource::Property>> properties;
	std::map<std::shared_ptr<Resource::Property>, PropInfo> propInfo;
}