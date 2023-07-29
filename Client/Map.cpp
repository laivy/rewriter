#include "Stdafx.h"
#include "Map.h"
#include "Platform.h"

Map::Map() : m_id{}
{

}

const std::vector<std::shared_ptr<Platform>>& Map::GetPlatforms() const
{
	return m_platforms;
}

std::weak_ptr<Platform> Map::GetBelowPlatform(const FLOAT2& position) const
{
	std::weak_ptr<Platform> topPlatform{};
	FLOAT topPlatformHeight{ -FLT_MAX };

	for (const auto& p : m_platforms)
	{
		const auto& [s, e] { p->GetStartEndPosition() };
		if (position.x < s.x || position.x > e.x)
			continue;

		FLOAT platformHeight{ p->GetHeight(position.x) };
		if (platformHeight > position.y ||
			platformHeight < topPlatformHeight)
			continue;

		topPlatformHeight = platformHeight;
		topPlatform = p;
	}
	return topPlatform;
}
