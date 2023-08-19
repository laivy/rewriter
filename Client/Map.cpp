#include "Stdafx.h"
#include "Map.h"
#include "Platform.h"

Map::Map() :
	m_id{},
	m_size{ 1920, 1080 }
{

}

void Map::Update(FLOAT deltaTime)
{
}

void Map::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (const auto& p : m_platforms)
		p->Render(commandList);
}

INT2 Map::GetSize() const
{
	return m_size;
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
