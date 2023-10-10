#pragma once
#include "GameObject.h"

class Platform;

class Map : public IGameObject
{
public:
	Map();
	~Map() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	INT2 GetSize() const;
	std::weak_ptr<Platform> GetBelowPlatform(const FLOAT2& position) const;

public:
	int m_id;
	INT2 m_size;
	std::vector<std::shared_ptr<Platform>> m_platforms;
};