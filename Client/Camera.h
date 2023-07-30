#pragma once
#include "ConstantBuffer.h"

class IGameObject;

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

	virtual void Update(FLOAT deltaTime);
	void SetShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);

	void SetScale(const FLOAT2& scale);
	void SetRotation(FLOAT degree);
	void SetPosition(const FLOAT2& position);

	FLOAT2 GetScale() const;
	FLOAT GetRotation() const;
	FLOAT2 GetPosition() const;

protected:
	// 카메라의 eye가 움직일 수 있는 범위를 반환
	RECTF GetCameraBoundary() const;

protected:
	DirectX::XMFLOAT3 m_eye;
	DirectX::XMFLOAT3 m_at;
	DirectX::XMFLOAT3 m_up;

	FLOAT2 m_scale;
	FLOAT m_degree;

	ConstantBuffer<cbCamera> m_cbCamera;
};

class FocusCamera : public Camera
{
public:
	FocusCamera();
	~FocusCamera() = default;

	virtual void Update(FLOAT deltaTime);

	void SetFocus(const std::shared_ptr<IGameObject>& focus);

private:
	std::weak_ptr<IGameObject> m_focus;
	float m_delay;
};