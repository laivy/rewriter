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
		FLOAT layer;
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

	void Move(const FLOAT2& delta);

	void SetLayer(Layer layer);
	void SetSize(const FLOAT2& size);
	void SetScale(const FLOAT2& scale);
	void SetRotation(FLOAT degree);
	void SetPosition(const FLOAT2& position, Pivot pivot = Pivot::LEFTTOP);
	void SetShader(Shader* shader);
	void SetMesh(Mesh* mesh);

	BOOL IsValid() const;
	FLOAT2 GetSize() const;
	FLOAT2 GetPosition() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;

protected:
	BOOL m_isValid;
	Layer m_layer;
	FLOAT2 m_size;
	FLOAT2 m_scale;
	FLOAT m_degree;
	FLOAT2 m_position;
	Pivot m_pivot;

	Shader* m_shader;
	Mesh* m_mesh;
	ConstantBuffer<cbGameObject> m_cbGameObject;
};