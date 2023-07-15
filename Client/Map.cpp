#include "Stdafx.h"
#include "Map.h"

const std::vector<std::shared_ptr<Platform>>& Map::GetPlatforms() const
{
	return m_platforms;
}
