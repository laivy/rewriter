#pragma once

namespace Graphics
{
	DLL_API bool Initialize(HWND hWnd);
	DLL_API void CleanUp();

	DLL_API void OnResize(int width, int height);
}
