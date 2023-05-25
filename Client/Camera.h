#pragma once
#include "ConstantBuffer.h"

class Camera
{
private:
	struct cbCamera
	{
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projMatrix;
	};

public:
	Camera();
	~Camera() = default;

	void Update(FLOAT deltaTime);
	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

	void SetScale(const FLOAT2& scale);
	void SetRotation(FLOAT degree);

	FLOAT2 GetScale() const;
	FLOAT GetRotation() const;
	FLOAT2 GetPosition() const;
	MATRIX GetMatrix() const;

private:
	DirectX::XMFLOAT3 m_eye;
	DirectX::XMFLOAT3 m_at;
	DirectX::XMFLOAT3 m_up;

	FLOAT2 m_scale;
	FLOAT m_degree;

	ConstantBuffer<cbCamera> m_cbCamera;
};