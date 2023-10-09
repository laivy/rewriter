#include "Stdafx.h"

BOOL APIENTRY DllMain(HMODULE	hModule,
					  DWORD		ul_reason_for_call,
					  LPVOID	lpReserved)
{
	using namespace Database;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (!Connection::IsInstanced())
		{
			Connection::Instantiate();
			Connection::GetInstance()->OnCreate();
		}
		break;
	case DLL_PROCESS_DETACH:
		if (!Connection::IsInstanced())
		{
			Connection::GetInstance()->OnDestroy();
			Connection::Destroy();
		}
		break;
	}
	return TRUE;
}