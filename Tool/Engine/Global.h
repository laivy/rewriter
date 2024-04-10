#pragma once

struct PropInfo;

namespace Global
{
	extern Event<std::shared_ptr<Resource::Property>> OnPropertyAdd;
	extern Event<std::shared_ptr<Resource::Property>> OnPropertyDelete;
	extern Event<std::shared_ptr<Resource::Property>> OnPropertySelect;

	extern std::vector<std::shared_ptr<Resource::Property>> properties;
	extern std::map<std::shared_ptr<Resource::Property>, PropInfo> propInfo;
}