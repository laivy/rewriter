module rewriter.library.graphics;

import rewriter.library.graphics.direct2d;
import rewriter.library.graphics.direct3d;
import rewriter.library.graphics.global;

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
