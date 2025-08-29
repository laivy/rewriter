#include "Stdafx.h"
#include "Manager.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Resource::Manager::Instantiate();
		break;
	case DLL_PROCESS_DETACH:
		Resource::Manager::Destroy();
		break;
	}
	return TRUE;
}
