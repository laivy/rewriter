#pragma once

class IScene abstract
{
public:
	IScene();
	virtual ~IScene() = default;

	virtual void Update(float deltaTime);
	virtual void Render2D() const;
	virtual void Render3D() const;
};