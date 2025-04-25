#include "Stdafx.h"
#include "Global.h"
#include "Graphics.h"
#include "Graphics2D.h"
#include "Graphics3D.h"

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
		D3D::OnResize(width, height);
		D2D::OnResize(width, height);
	}
}
