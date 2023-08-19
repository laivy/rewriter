#pragma once
#include "ConstantBuffer.h"
#include "GameObject.h"

class Camera : public IGameObject
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

	virtual void SetScale(const FLOAT2& scale);
	virtual void SetRotation(FLOAT degree);

protected:
	// 카메라의 eye가 움직일 수 있는 범위를 반환
	RECTF GetCameraBoundary() const;

protected:
	FLOAT2 m_at;
	FLOAT2 m_up;

	ConstantBuffer<cbCamera> m_cbCamera;
};

class FocusCamera : public Camera
{
public:
	FocusCamera();
	~FocusCamera() = default;

	virtual void Update(FLOAT deltaTime);

	void SetFocus(const std::weak_ptr<IGameObject>& focus);

private:
	std::weak_ptr<IGameObject> m_focus;
	float m_delay;
};