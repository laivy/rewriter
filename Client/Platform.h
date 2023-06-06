#pragma once
#include "GameObject.h"

class Platform : public IGameObject
{
public:
	Platform(INT2 startPosition, INT2 endPosition);
	~Platform() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;

	std::pair<INT2, INT2> GetStartEndPosition() const;
	FLOAT GetHeight(FLOAT x) const;

private:
	INT2 m_startPosition;
	INT2 m_endPosition;
};