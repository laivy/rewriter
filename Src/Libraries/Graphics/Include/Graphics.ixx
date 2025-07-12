export module Library.Graphics;

#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

export namespace Graphics
{
	GRAPHICS_API bool Initialize(void* hWnd);
	GRAPHICS_API void CleanUp();

	GRAPHICS_API void OnWindowResized(int width, int height);
}
