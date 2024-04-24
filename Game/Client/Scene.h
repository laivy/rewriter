#pragma once

class IScene abstract
{
public:
	IScene() = default;
	virtual ~IScene() = default;

	virtual void OnCreate();
	virtual void OnDestroy();

	virtual void Update(float deltaTime);
	virtual void Render2D() const;
	virtual void Render3D() const;

private:

};