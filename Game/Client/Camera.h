#pragma once
#include "ConstantBuffer.h"

class Camera
{
private:
	struct ShaderVariable
	{
		static constexpr auto INDEX = 1u;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projMatrix;
	};

public:
	Camera();
	~Camera() = default;

	void Update(float deltaTime);

	void SetShaderVariable();
	void SetScale(const FLOAT2& scale);
	void SetRotation(float degree);

private:
	void OnResize(int width, int height);

protected:
	FLOAT2 m_eye;
	FLOAT2 m_at;
	FLOAT2 m_up;
	FLOAT2 m_scale;
	float m_degree;

private:
	ConstantBuffer<ShaderVariable> m_cbCamera;
};