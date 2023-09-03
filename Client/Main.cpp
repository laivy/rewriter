#include "Stdafx.h"
#include "GameApp.h"

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	GameApp::Instantiate();
	if (GameApp::IsInstanced())
	{
		GameApp::GetInstance()->OnCreate();
		GameApp::GetInstance()->Run();
	}
	GameApp::Destroy();

#ifdef _DEBUG
	ComPtr<IDXGIDebug1> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS::DXGI_DEBUG_RLO_ALL);
	}
#endif
}