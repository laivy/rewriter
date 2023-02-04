#pragma once

class Camera;

struct NytRenderTarget
{
	ComPtr<ID2D1HwndRenderTarget> m_renderTarget;
	Camera* m_camera;
};