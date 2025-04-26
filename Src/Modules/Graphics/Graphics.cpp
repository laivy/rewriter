#include "Stdafx.h"
#include "Global.h"
#include "Graphics.h"
#include "Graphics2D.h"
#include "Graphics3D.h"
#include "SwapChain.h"

namespace Graphics
{
	DLL_API bool Initialize(HWND hWnd)
	{
		g_hWnd = hWnd;
		if (!D3D::Initialize())
			return false;
		if (!D2D::Initialize())
			return false;
		return true;
	}

	DLL_API void CleanUp()
	{
		D3D::CleanUp();
		D2D::CleanUp();
	}

	DLL_API void OnResize(int width, int height)
	{
		g_swapChain->WaitForPreviousFrame();

		g_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
		g_scissorRect = { 0, 0, static_cast<long>(width), static_cast<long>(height) };
		g_swapChain->Resize(width, height);
		g_renderTargetSize = Int2{ width, height };
	}
}
