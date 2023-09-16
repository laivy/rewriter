#include "Stdafx.h"
#include "LoginApp.h"

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	LoginApp::Instantiate();
	if (LoginApp::IsInstanced())
	{
		LoginApp::GetInstance()->OnCreate();
		LoginApp::GetInstance()->Run();
	}
	LoginApp::Destroy();
}