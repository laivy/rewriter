#pragma once

class Platform;

class Map
{
public:
	Map();
	~Map() = default;

	const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const;

	std::weak_ptr<Platform> GetBelowPlatform(const FLOAT2& position) const;

public:
	int m_id;
	std::vector<std::shared_ptr<Platform>> m_platforms;
};