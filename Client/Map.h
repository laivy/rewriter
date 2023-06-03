#pragma once

class Platform;

class Map
{
public:
	Map() = default;
	~Map() = default;

public:
	int m_id;
	std::vector<Platform> m_platforms;
};