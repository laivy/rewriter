#pragma once
#include "ConstantBuffer.h"

class Shader;
class Mesh;

class IGameObject abstract
{
private:
	struct cbGameObject
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		INT layer;
		FLOAT alpha;
		BOOL isFliped;
		FLOAT dummy;
	};

public:
	IGameObject();
	virtual ~IGameObject() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Destroy();

	virtual void Move(const FLOAT2& delta);

	virtual void SetLayer(Layer layer);
	virtual void SetPivot(Pivot pivot);
	virtual void SetSize(const FLOAT2& size);
	virtual void SetScale(const FLOAT2& scale);
	virtual void SetRotation(FLOAT degree);
	virtual void SetPosition(const FLOAT2& position);

	bool IsValid() const;
	FLOAT2 GetSize() const;
	FLOAT2 GetPosition() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;

protected:
	bool m_isValid;
	Layer m_layer;
	Pivot m_pivot;
	FLOAT2 m_size;
	FLOAT2 m_scale;
	float m_degree;
	FLOAT2 m_position;

	std::weak_ptr<Shader> m_shader;
	std::weak_ptr<Mesh> m_mesh;
	ConstantBuffer<cbGameObject> m_cbGameObject;
};