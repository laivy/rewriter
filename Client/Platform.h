#pragma once
#include "GameObject.h"

class Platform : public IGameObject
{
public:
	Platform(INT2 startPosition, INT2 endPosition);
	~Platform() = default;

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const override;

	std::pair<INT2, INT2> GetStartEndPosition() const;
	float GetHeight(FLOAT x) const;
	bool IsBetweenX(float x) const;
	bool IsBetweenY(float y) const;

private:
	INT2 m_startPosition;
	INT2 m_endPosition;

#ifdef _DEBUG
	struct cbLine
	{
		DirectX::XMFLOAT2 position1;
		DirectX::XMFLOAT2 position2;
		DirectX::XMFLOAT4 color;
	};
	ConstantBuffer<cbLine> m_cbLine;
#endif
};