#include "Stdafx.h"
#include "Session.h"

namespace Database
{
	std::map<Type, std::unique_ptr<Session>> g_sessions;

	Session::Session(std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password) :
		m_env{ SQL_NULL_HANDLE },
		m_dbc{ SQL_NULL_HANDLE }
	{
		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_env)))
		{
			assert(false);
			return;
		}
		if (!SQL_SUCCEEDED(::SQLSetEnvAttr(m_env, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0)))
		{
			assert(false);
			return;
		}
		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_DBC, m_env, &m_dbc)))
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

		if (!SQL_SUCCEEDED(::SQLDriverConnect(m_dbc, NULL, connStr.data(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE)))
		{
			assert(false);
			return;
		}
	}

	Session::~Session()
	{
		if (m_dbc)
		{
			::SQLDisconnect(m_dbc);
			::SQLFreeHandle(SQL_HANDLE_DBC, m_dbc);
		}
		if (m_env)
		{
			::SQLFreeHandle(SQL_HANDLE_ENV, m_env);
		}
	}

	Session* Connect(Type type, std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password)
	{
		g_sessions.emplace(type, std::make_unique<Session>(server, database, username, password));
		return g_sessions.at(type).get();
	}

	Session* GetSession(Type type)
	{
		return g_sessions.at(type).get();
	}
}
