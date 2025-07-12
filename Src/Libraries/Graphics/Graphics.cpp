module Library.Graphics;

import Library.Graphics.D2D;
import Library.Graphics.D3D;
import Library.Graphics.Global;

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

	void CleanUp()
	{
		D3D::CleanUp();
#ifdef _DIRECT2D
		D2D::CleanUp();
#endif
	}

	void OnWindowResized(int width, int height)
	{
		g_swapChain->OnResize(width, height);
	}
}
