#include "Stdafx.h"
#include "NytApp.h"

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_ HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	NytApp::Instantiate();
	if (NytApp::IsInstanced())
		NytApp::GetInstance()->Run();
}