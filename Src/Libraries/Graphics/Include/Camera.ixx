export module rewriter.library.graphics.direct3d:camera;

import std;
import rewriter.common.type;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics::D3D
{
	struct Camera;

	GRAPHICS_API std::shared_ptr<Camera> NewCamera();
	GRAPHICS_API std::shared_ptr<Camera> GetCamera();
	GRAPHICS_API void SetCamera(const std::shared_ptr<Camera>& camera);
	GRAPHICS_API void SetCameraPosition(const std::shared_ptr<Camera>& camera, Int2 position);
	GRAPHICS_API void SetCameraScale(const std::shared_ptr<Camera>& camera, float scale);
	GRAPHICS_API void SetCameraShaderConstants(const std::shared_ptr<Camera>& camera);
}
