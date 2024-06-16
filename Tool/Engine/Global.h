#pragma once

struct PropInfo;

namespace Global
{
	extern std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertyAdd;
	extern std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertyDelete;
	extern std::shared_ptr<Event<std::shared_ptr<Resource::Property>>> OnPropertySelect;

	extern std::vector<std::shared_ptr<Resource::Property>> properties;
	extern std::map<std::shared_ptr<Resource::Property>, PropInfo> propInfo;
}