#pragma once

class Camera
{
public:
	Camera();
	~Camera() = default;

	void Update(FLOAT deltaTime);

	void SetScale(const FLOAT2& scale);
	void SetRotation(FLOAT degree);
	void SetPosition(const FLOAT2& position);

	FLOAT2 GetScale() const;
	FLOAT GetRotation() const;
	FLOAT2 GetPosition() const;
	MATRIX GetMatrix() const;

private:
	FLOAT2 m_scale;
	FLOAT m_degree;
	FLOAT2 m_position;
};