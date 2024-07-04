#include "Stdafx.h"
#include "Manager.h"

BOOL APIENTRY DllMain(HMODULE	hModule,
					  DWORD		ul_reason_for_call,
					  LPVOID	lpReserved)
{
	using namespace Resource;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		if (!Manager::IsInstanced())
			Manager::Instantiate();
		break;
    case DLL_PROCESS_DETACH:
		if (Manager::IsInstanced())
			Manager::Destroy();
        break;
    }
    return TRUE;
}

