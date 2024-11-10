#include "Stdafx.h"
#include "Connection.h"

namespace Database
{
	Connection::Connection(std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password) :
		m_hEnv{ SQL_NULL_HANDLE },
		m_hDbc{ SQL_NULL_HANDLE }
	{
		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv)))
		{
			assert(false);
			return;
		}
		if (!SQL_SUCCEEDED(::SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0)))
		{
			assert(false);
			return;
		}
		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc)))
		{
			assert(false);
			return;
		}

		auto connStr{ std::format(
				L"DRIVER={{SQL Server}};"
				"TRUSTSERVERCERTIFICATE=yes;"
				"SERVER={};"
				"DATABASE={};"
				"UID={};"
				"PWD={};",
				server, database, username, password
		) };

		if (!SQL_SUCCEEDED(::SQLDriverConnect(m_hDbc, NULL, connStr.data(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE)))
		{
			assert(false);
			return;
		}
	}

	Connection::~Connection()
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
}
