#pragma once
#include "ConstantBuffer.h"

class Shader;
class Mesh;

class IGameObject abstract
{
private:
	struct cbGameObject
	{
		DirectX::XMFLOAT4X4 worldMatrix
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		Layer layer{ Layer::COUNT };
		FLOAT alpha{ 1.0f };
		BOOL isFliped{ FALSE };
		FLOAT dummy{ 0.0f };
	};

public:
	enum class Direction
	{
		LEFT = -1,
		NONE = 0,
		RIGHT = 1
	};

public:
	IGameObject();
	virtual ~IGameObject() = default;

	virtual void Update(FLOAT deltaTime) = 0;
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const = 0;
	virtual void Destroy();

	virtual void Move(const FLOAT2& delta);

	virtual void SetLayer(Layer layer);
	virtual void SetPivot(Pivot pivot);
	virtual void SetSize(const FLOAT2& size);
	virtual void SetScale(const FLOAT2& scale);
	virtual void SetRotation(FLOAT degree);
	virtual void SetPosition(const FLOAT2& position);
	virtual void SetDirection(Direction direction);

	bool IsValid() const;
	FLOAT2 GetSize() const;
	FLOAT2 GetPosition() const;
	Direction GetDirection() const;
	DirectX::XMFLOAT4X4 GetWorldMatrix() const;

protected:
	bool m_isValid;
	Layer m_layer;
	Pivot m_pivot;
	FLOAT2 m_size;
	FLOAT2 m_scale;
	float m_degree;
	FLOAT2 m_position;
	FLOAT2 m_speed;
	Direction m_direction; // 바라보는 방향

	std::weak_ptr<Shader> m_shader;
	std::weak_ptr<Mesh> m_mesh;
	ConstantBuffer<cbGameObject> m_cbGameObject;
};