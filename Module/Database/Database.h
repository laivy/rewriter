#pragma once

namespace Resource
{
	class Property;
}

namespace Database
{
	DLL_API void Initialize(const std::shared_ptr<Resource::Property>& prop);
}
