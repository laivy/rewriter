#include "Stdafx.h"
#include "Include/ResourceManager.h"

BOOL APIENTRY DllMain(HMODULE	hModule,
					  DWORD		ul_reason_for_call,
					  LPVOID	lpReserved)
{
	using namespace Resource;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		if (!ResourceManager::IsInstanced())
			ResourceManager::Instantiate();
		break;
    case DLL_PROCESS_DETACH:
		if (ResourceManager::IsInstanced())
			ResourceManager::Destroy();
        break;
    }
    return TRUE;
}

