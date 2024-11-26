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
	void SetScale(const Float2& scale);
	void SetRotation(float degree);

private:
	void OnResize(int width, int height);

protected:
	Float2 m_eye;
	Float2 m_at;
	Float2 m_up;
	Float2 m_scale;
	float m_degree;

private:
	ConstantBuffer<ShaderVariable> m_cbCamera;
};