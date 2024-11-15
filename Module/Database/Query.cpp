#include "Stdafx.h"
#include "Query.h"
#include "Session.h"

namespace
{
	void Debug(SQLHANDLE handle, SQLSMALLINT handleType)
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

	void FreeHandle(void** ptr)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, *ptr);
	}
}

namespace Database
{
	DLL_API Select::Select(Type type)
	{
		m_session = GetSession(type);
		if (!m_session)
		{
			assert(false);
			return;
		}

		SQLHSTMT stmt{};
		if (!SQL_SUCCEEDED(::SQLAllocHandle(SQL_HANDLE_STMT, m_session->m_dbc, &stmt)))
		{
			assert(false);
			return;
		}
		m_stmt = unique_stmt{ new SQLHSTMT{ stmt }, FreeHandle };
	}

	DLL_API Select::Select(Select& select) :
		m_session{ select.m_session },
		m_stmt{ std::move(select.m_stmt) }
	{
	}

	DLL_API Select& Select::Statement(std::wstring_view statement)
	{
		if (!SQL_SUCCEEDED(::SQLPrepare(*m_stmt, const_cast<wchar_t*>(statement.data()), SQL_NTS)))
			assert(false);
		return *this;
	}

	DLL_API Select& Select::Param(unsigned short number, int32_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &param, 0, nullptr)))
			assert(false);
		return *this;
	}

	DLL_API Select& Select::Param(unsigned short number, int64_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &param, 0, nullptr)))
			assert(false);
		return *this;
	}

	DLL_API Select& Select::Param(unsigned short number, std::wstring_view param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, param.size(), 0, const_cast<wchar_t*>(param.data()), param.size(), nullptr)))
			assert(false);
		return *this;
	}

	DLL_API Select::Result Select::Execute()
	{
		if (!m_stmt)
		{
			assert(false);
			return Result{ nullptr };
		}

		auto ret = ::SQLExecute(*m_stmt);
		if (!SQL_SUCCEEDED(ret))
		{
			assert(false);
			return Result{ nullptr };
		}

		return Result{ std::move(m_stmt) };
	}

	Select::Result::Result(unique_stmt stmt)
	{
		m_stmt.swap(stmt);
	}

	DLL_API Select::Result& Select::Result::Bind(unsigned short number, int32_t* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_LONG, param, 0, nullptr);
		return *this;
	}

	DLL_API Select::Result& Select::Result::Bind(unsigned short number, int64_t* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_SBIGINT, param, 0, nullptr);
		return *this;
	}

	DLL_API Select::Result& Select::Result::Bind(unsigned short number, std::wstring* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_WCHAR, param->data(), param->size(), nullptr);
		return *this;
	}

	DLL_API Select::Result& Select::Result::Bind(unsigned short number, Time* param)
	{
		m_datetimes.emplace(param, datetime{});
		::SQLBindCol(*m_stmt, number, SQL_C_TIMESTAMP, &m_datetimes[param], 0, nullptr);
		return *this;
	}

	DLL_API bool Select::Result::Fetch()
	{
		auto result{ ::SQLFetch(*m_stmt) };
		for (auto& [time, datetime] : m_datetimes)
		{
			auto timestamp{ *reinterpret_cast<SQL_TIMESTAMP_STRUCT*>(&datetime) };
			*time = Time{
				timestamp.year,
				timestamp.month,
				timestamp.day,
				timestamp.hour,
				timestamp.minute,
				timestamp.second,
			};
		}
		return SQL_SUCCEEDED(result);
	}
}
