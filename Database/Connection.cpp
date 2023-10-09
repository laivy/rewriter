#include "Stdafx.h"
#include "include/Connection.h"

namespace Database
{
	Connection::Connection() :
		m_hEnv{},
		m_hDbc{}
	{
	}

	void Connection::OnCreate()
	{
		SQLRETURN rc{ SQL_SUCCESS };
		rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, std::addressof(m_hEnv));
		rc = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), NULL);
		rc = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, std::addressof(m_hDbc));
	}

	void Connection::OnDestroy()
	{
		if (m_hDbc)
		{
			SQLDisconnect(m_hDbc);
			SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		}
		if (m_hEnv)
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
	}

	DLLEXPORT DBRESULT Connect(const std::wstring& server, const std::wstring& dbnamae, const std::wstring& username, const std::wstring& password)
	{
		auto conn{ Connection::GetInstance() };
		if (!conn)
			return DBRESULT::ERR_NOT_INITIALIZED;

		SQLRETURN rc{ SQL_SUCCESS };

		std::wstring connStr{};
		std::vformat_to(
			std::back_inserter(connStr),
			TEXT("DRIVER={{SQL Server}};"
				 "TRUSTSERVERCERTIFICATE=yes;"
				 "SERVER={};"
				 "DATABASE={};"
				 "UID={};"
				 "PWD={};"),
			std::make_wformat_args(server, dbnamae, username, password)
		);

		rc = SQLDriverConnect(conn->m_hDbc, NULL, const_cast<SQLWCHAR*>(connStr.c_str()), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
		if (!SQL_SUCCEEDED(rc))
			return DBRESULT::ERR_CAN_NOT_CONNECT;

		//SQLHSTMT hStmt{};
		//rc = SQLAllocHandle(SQL_HANDLE_STMT, conn->m_hDbc, std::addressof(hStmt));
		//if (!SQL_SUCCEEDED(rc))
		//	return DBRESULT::ERR_CAN_NOT_CONNECT;

		//std::wstring sql{ TEXT("SELECT * FROM Account") };
		//rc = SQLExecDirect(hStmt, const_cast<SQLWCHAR*>(sql.c_str()), SQL_NTS);

		//SQLINTEGER aid{};
		//SQLWCHAR id[16]{};
		//SQLCHAR pw[128]{};
		//
		//SQL_DATE_STRUCT regDate{};
		//rc = SQLBindCol(hStmt, 1, SQL_INTEGER, std::addressof(aid), sizeof(aid), NULL);
		//rc = SQLBindCol(hStmt, 2, SQL_WCHAR, std::addressof(id), sizeof(id), NULL);
		//rc = SQLBindCol(hStmt, 3, SQL_BINARY, std::addressof(pw), sizeof(pw), NULL);
		//rc = SQLBindCol(hStmt, 4, SQL_DATE, std::addressof(regDate), sizeof(regDate), NULL);
		//while (SQLFetch(hStmt) != SQL_NO_DATA)
		//{
		//	auto str{ std::vformat(TEXT("aid : {}, id : {}, regdate : {}-{}-{}\n"), std::make_wformat_args(aid, id, regDate.year, regDate.month, regDate.day)) };
		//	OutputDebugString(str.c_str());
		//}

		return DBRESULT::SUCCESS;
	}
}