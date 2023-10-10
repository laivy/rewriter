#include "Stdafx.h"
#include "ClientApp.h"

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	ClientApp::Instantiate();
	if (auto app{ ClientApp::GetInstance() })
	{
		app->OnCreate();
		app->Run();
	}
	ClientApp::Destroy();
}