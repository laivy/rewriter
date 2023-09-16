#include "Stdafx.h"

BOOL APIENTRY DllMain(HMODULE	hModule,
					  DWORD		ul_reason_for_call,
					  LPVOID	lpReserved)
{
	if (!Database::Connection::IsInstanced())
	{
		Database::Connection::Instantiate();
		Database::Connection::GetInstance()->OnCreate();
	}
	return TRUE;
}