#include "Stdafx.h"
#include "Query.h"
#include "Session.h"

static void Debug(SQLHANDLE handle, SQLSMALLINT handleType)
{
	SQLWCHAR sqlState[6]{};
	SQLINTEGER nativeError{};
	SQLWCHAR messageText[256]{};
	SQLSMALLINT textLength{};

	int i{ 1 };
	while (::SQLGetDiagRec(handleType, handle, i, sqlState, &nativeError, messageText, sizeof(messageText), &textLength) != SQL_NO_DATA)
	{
		++i;
	}
};

namespace Database
{
	DLL_API Query::Query(Type type) :
		m_session{ nullptr },
		m_stmt{ SQL_NULL_HANDLE }
	{
		m_session = GetSession(type);
		if (!m_session)
		{
			assert(false);
			return;
		}

		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_STMT, m_session->m_dbc, &m_stmt)))
		{
			assert(false);
			return;
		}
	}

	DLL_API Query::~Query()
	{
		if (m_stmt)
			::SQLFreeHandle(SQL_HANDLE_STMT, m_stmt);
	}

	DLL_API Query& Query::Statement(std::wstring statement)
	{
		if (!m_stmt)
		{
			assert(false);
			return *this;
		}

		if (!SQL_SUCCEEDED(::SQLPrepare(m_stmt, statement.data(), SQL_NTS)))
		{
			assert(false);
			return *this;
		}

		return *this;
	}

	DLL_API Query& Query::Param(size_t number, int32_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &param, 0, nullptr)))
		{
			assert(false);
			return *this;
		}
		return *this;
	}

	DLL_API Query& Query::Param(size_t number, int64_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &param, 0, nullptr)))
		{
			assert(false);
			return *this;
		}
		return *this;
	}

	DLL_API Query& Query::Param(size_t number, const std::wstring& param)
	{
		if (!m_stmt)
		{
			assert(false);
			return *this;
		}

		if (number < 1)
		{
			assert(false);
			return *this;
		}

		if (!SQL_SUCCEEDED(::SQLBindParameter(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, param.size(), 0, const_cast<wchar_t*>(param.data()), param.size(), nullptr)))
		{
			Debug(m_stmt, SQL_HANDLE_STMT);
			assert(false);
			return *this;
		}

		return *this;
	}

	DLL_API bool Query::Execute() const
	{
		if (!m_stmt)
		{
			assert(false);
			return false;
		}

		if (!SQL_SUCCEEDED(::SQLExecute(m_stmt)))
		{
			Debug(m_stmt, SQL_HANDLE_STMT);
			return false;
		}

		return true;
	}

	DLL_API Query& Query::Bind(size_t number, int32_t* param)
	{
		if (!SQL_SUCCEEDED(::SQLBindCol(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_LONG, param, 0, nullptr)))
		{
			assert(false);
			return *this;
		}
		return *this;
	}

	DLL_API Query& Query::Bind(size_t number, int64_t* param)
	{
		if (!SQL_SUCCEEDED(::SQLBindCol(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_SBIGINT, param, 0, nullptr)))
		{
			assert(false);
			return *this;
		}
		return *this;
	}

	DLL_API Query& Query::Bind(size_t number, std::wstring* param)
	{
		if (!SQL_SUCCEEDED(::SQLBindCol(m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_WCHAR, param->data(), param->size(), nullptr)))
		{
			assert(false);
			return *this;
		}
		return *this;
	}

	DLL_API bool Query::Fetch() const
	{
		auto ret{ ::SQLFetch(m_stmt) };
		Debug(m_stmt, SQL_HANDLE_STMT);
		return SQL_SUCCEEDED(ret);
	}
}
