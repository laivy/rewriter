#include "Stdafx.h"

BOOL APIENTRY DllMain(HMODULE	hModule,
					  DWORD		ul_reason_for_call,
					  LPVOID	lpReserved)
{
	Delegate<int> test;
	test.Register(
		{},
		[](int i) { ::OutputDebugString(L"callback"); }
	);
	test.Notify(1);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}