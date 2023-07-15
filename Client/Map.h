#pragma once

class Platform;

class Map
{
public:
	Map() = default;
	~Map() = default;

	const std::vector<std::shared_ptr<Platform>>& GetPlatforms() const;

public:
	int m_id;
	std::vector<std::shared_ptr<Platform>> m_platforms;
};