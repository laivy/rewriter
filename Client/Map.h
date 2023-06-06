#pragma once

class Platform;

class Map
{
public:
	Map() = default;
	~Map() = default;

	const std::vector<Platform>& GetPlatforms() const;

public:
	int m_id;
	std::vector<Platform> m_platforms;
};