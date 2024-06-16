#include "Stdafx.h"
#include "Global.h"
#include "PropInfo.h"

namespace Global
{
	std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertyAdd{ std::make_shared<Event<std::shared_ptr<Resource::Property>>>() };
	std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertyDelete{ std::make_shared<Event<std::shared_ptr<Resource::Property>>>() };
	std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertySelect{ std::make_shared<Event<std::shared_ptr<Resource::Property>>>() };

	std::vector<std::shared_ptr<Resource::Property>> properties;
	std::map<std::shared_ptr<Resource::Property>, PropInfo> propInfo;
}