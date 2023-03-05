#pragma once
#include "ConstantBuffer.h"

class Shader;
class Mesh;

class GameObject abstract
{
public:
	GameObject();
	virtual ~GameObject() = default;

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList> commandList) const;
	virtual void Destroy();

	void SetSize(const FLOAT2& size);
	void SetScale(const FLOAT2& scale);
	void SetRotation(FLOAT degree);
	void SetPosition(const FLOAT2& position, Pivot pivot = Pivot::LEFTTOP);
	void SetPosition(FLOAT x, FLOAT y, Pivot pivot = Pivot::LEFTTOP);
	void SetShader(Shader* shader);
	void SetMesh(Mesh* mesh);

	INT GetId() const;
	BOOL IsValid() const;
	FLOAT2 GetSize() const;
	FLOAT2 GetPosition() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;

protected:
	BOOL m_isValid;
	FLOAT2 m_size;
	FLOAT2 m_scale;
	FLOAT m_degree;
	FLOAT2 m_position;

	Shader* m_shader;
	Mesh* m_mesh;

	struct cbGameObject
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		FLOAT alpha;
		BOOL isFliped;
		FLOAT2 dummy;
	};
	ConstantBuffer<cbGameObject> m_cbGameObject;

private:
	static INT s_id;
	INT m_id;
};