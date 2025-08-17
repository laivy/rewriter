#include "Stdafx.h"
#include "Global.h"
#include "Graphics.h"
#include "Graphics2D.h"
#include "Graphics3D.h"
#include "SwapChain.h"

namespace Graphics
{
	bool Initialize(void* hWnd)
	{
		g_hWnd = hWnd;
		if (!D3D::Initialize())
			return false;
#ifdef _DIRECT2D
		if (!D2D::Initialize())
			return false;
#endif
		return true;
	}

	void Uninitialize()
	{
		D3D::Uninitialize();
#ifdef _DIRECT2D
		D2D::Uninitialize();
#endif
	}

	void OnWindowResized(int width, int height)
	{
		g_swapChain->OnResize(width, height);
	}
}
