#include <Windows.h>

import rewriter.tool.engine.app;

int WINAPI WinMain(_In_		HINSTANCE hInstance,
				   _In_opt_	HINSTANCE hPrevInstance,
				   _In_		LPSTR lpCmdLine,
				   _In_		int nCmdShow)
{
	if (auto app{ App::Instantiate() })
		app->Run();
	App::Destroy();
}
