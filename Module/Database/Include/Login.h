#pragma once

namespace Database
{
	DLLEXPORT std::pair<DBRESULT, SPRESULT> Register(const std::wstring& id, const std::wstring& password);
	DLLEXPORT std::pair<DBRESULT, SPRESULT> Login(const std::wstring& id, const std::wstring& password);
}