#include "Stdafx.h"
#include "Connection.h"

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
		rc = ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, std::addressof(m_hEnv));
		rc = ::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), NULL);
		rc = ::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, std::addressof(m_hDbc));
	}

	void Connection::OnDestroy()
	{
		if (m_hDbc)
		{
			::SQLDisconnect(m_hDbc);
			::SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		}
		if (m_hEnv)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		}
	}

	__declspec(dllexport) DBRESULT Connect(std::wstring_view server, std::wstring_view dbnamae, std::wstring_view username, std::wstring_view password)
	{
		auto conn{ Connection::GetInstance() };
		if (!conn)
			return DBRESULT::ERR_NOT_INITIALIZED;

		std::wstring connStr{
			std::format(
				L"DRIVER={{SQL Server}};"
				"TRUSTSERVERCERTIFICATE=yes;"
				"SERVER={};"
				"DATABASE={};"
				"UID={};"
				"PWD={};",
				server, dbnamae, username, password
			)
		};

		SQLRETURN rc{ SQL_SUCCESS };
		rc = ::SQLDriverConnect(conn->m_hDbc, NULL, connStr.data(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
		if (!SQL_SUCCEEDED(rc))
			return DBRESULT::ERR_CAN_NOT_CONNECT;

		return DBRESULT::SUCCESS;
	}
}