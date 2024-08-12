#include "Stdafx.h"
#include "Connection.h"
#include "Login.h"

namespace Database
{
	__declspec(dllexport) std::pair<DBRESULT, SPRESULT> Register(const std::wstring& id, const std::wstring& password)
	{
		auto conn{ Connection::GetInstance() };
		if (!conn)
			return { DBRESULT::ERR_NOT_INITIALIZED, -1 };

		SQLRETURN rc{ SQL_SUCCESS };
		SQLHSTMT hStmt{};
		rc = SQLAllocHandle(SQL_HANDLE_STMT, conn->m_hDbc, std::addressof(hStmt));
		if (!SQL_SUCCEEDED(rc))
			return { DBRESULT::ERR_CAN_NOT_ALLOC_HANDLE, -1 };

		SPRESULT spResult{ -1 };
		rc = SQLBindParameter(hStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &spResult, 0, NULL);
		rc = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(WCHAR) * id.size(), 0, const_cast<WCHAR*>(id.c_str()), 0, NULL);
		rc = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(WCHAR) * password.size(), 0, const_cast<WCHAR*>(password.c_str()), 0, NULL);
		rc = SQLExecDirect(hStmt, const_cast<SQLWCHAR*>(TEXT("{ ? = CALL Register(?, ?) }")), SQL_NTS);
		return { DBRESULT::SUCCESS, spResult };
	}

	__declspec(dllexport) std::pair<DBRESULT, SPRESULT> Login(const std::wstring& id, const std::wstring& password)
	{
		auto conn{ Connection::GetInstance() };
		if (!conn)
			return { DBRESULT::ERR_NOT_INITIALIZED, -1 };

		SQLRETURN rc{ SQL_SUCCESS };
		SQLHSTMT hStmt{};
		rc = SQLAllocHandle(SQL_HANDLE_STMT, conn->m_hDbc, std::addressof(hStmt));
		if (!SQL_SUCCEEDED(rc))
			return { DBRESULT::ERR_CAN_NOT_ALLOC_HANDLE, -1 };
		
		SPRESULT spResult{ -1 };
		rc = SQLBindParameter(hStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0, &spResult, 0, NULL);
		rc = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(WCHAR) * id.size(), 0, const_cast<WCHAR*>(id.c_str()), 0, NULL);
		rc = SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(WCHAR) * password.size(), 0, const_cast<WCHAR*>(password.c_str()), 0, NULL);
		rc = SQLExecDirect(hStmt, const_cast<SQLWCHAR*>(TEXT("{ ? = CALL Login(?, ?) }")), SQL_NTS);
		return { DBRESULT::SUCCESS, spResult };
	}
}