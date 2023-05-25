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
}