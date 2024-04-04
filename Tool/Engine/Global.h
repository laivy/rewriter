#pragma once

namespace Global
{
	extern Event<std::shared_ptr<Resource::Property>> OnPropertyAdd;
	extern Event<std::shared_ptr<Resource::Property>> OnPropertyDelete;
	extern Event<std::shared_ptr<Resource::Property>> OnPropertySelect;
}