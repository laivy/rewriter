#include "Stdafx.h"
#include "App.h"

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_	HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	if (auto app{ App::Instantiate() })
		app->Run();
	App::Destroy();
}