#pragma once

class Scene abstract
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void OnCreate();
	virtual void OnDestory();

	virtual void Update(FLOAT deltaTime);
	virtual void Render(const ComPtr<ID2D1HwndRenderTarget>& renderTarget) const;
};