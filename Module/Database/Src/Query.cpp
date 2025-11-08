#include "Pch.h"
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
	Query::Query(Type type)
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

	Query::Query(Query& query) :
		m_session{ query.m_session },
		m_stmt{ std::move(query.m_stmt) }
	{
	}

	Query& Query::Statement(std::wstring_view statement)
	{
		if (!SQL_SUCCEEDED(::SQLPrepare(*m_stmt, const_cast<wchar_t*>(statement.data()), SQL_NTS)))
			assert(false);
		return *this;
	}

	Query& Query::Param(unsigned short number, int32_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &param, 0, nullptr)))
			assert(false);
		return *this;
	}

	Query& Query::Param(unsigned short number, int64_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &param, 0, nullptr)))
			assert(false);
		return *this;
	}

	Query& Query::Param(unsigned short number, std::wstring_view param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, param.size(), 0, const_cast<wchar_t*>(param.data()), param.size(), nullptr)))
			assert(false);
		return *this;
	}

	Query& Query::Param(unsigned short number, const Time& param)
	{
		SQL_TIMESTAMP_STRUCT timeStamp{};
		timeStamp.year = param.Year();
		timeStamp.month = param.Month();
		timeStamp.day = param.Day();
		timeStamp.hour = param.Hour();
		timeStamp.minute = param.Min();
		timeStamp.second = param.Sec();
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, sizeof(timeStamp), 0, &timeStamp, 0, nullptr)))
			assert(false);
		return *this;
	}

	Query::Result Query::Execute()
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

	Query::Result::Result(unique_stmt stmt)
	{
		m_stmt.swap(stmt);
	}

	std::int64_t Query::Result::Return()
	{
		SQLLEN len{};
		if (!SQL_SUCCEEDED(::SQLRowCount(*m_stmt, &len)))
			return 0;
		return static_cast<std::int64_t>(len);
	}

	Query::Result& Query::Result::Bind(unsigned short number, int32_t* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_LONG, param, 0, nullptr);
		return *this;
	}

	Query::Result& Query::Result::Bind(unsigned short number, int64_t* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_SBIGINT, param, 0, nullptr);
		return *this;
	}

	Query::Result& Query::Result::Bind(unsigned short number, std::wstring* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_WCHAR, param->data(), param->size(), nullptr);
		return *this;
	}

	Query::Result& Query::Result::Bind(unsigned short number, Time* param)
	{
		m_datetimes.emplace(param, datetime2{});
		::SQLBindCol(*m_stmt, number, SQL_C_TIMESTAMP, &m_datetimes[param], 0, nullptr);
		return *this;
	}

	bool Query::Result::Fetch()
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

	StoredProcedure::StoredProcedure(Type type)
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

	StoredProcedure::StoredProcedure(StoredProcedure& sp) :
		m_session{ sp.m_session },
		m_stmt{ std::move(sp.m_stmt) }
	{
	}

	StoredProcedure& StoredProcedure::Statement(std::wstring_view statement)
	{
		if (!SQL_SUCCEEDED(::SQLPrepare(*m_stmt, const_cast<wchar_t*>(statement.data()), SQL_NTS)))
			assert(false);
		return *this;
	}

	StoredProcedure& StoredProcedure::In(unsigned short number, int64_t param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &param, 0, nullptr)))
			assert(false);
		return *this;
	}

	DATABASE_API StoredProcedure& StoredProcedure::In(unsigned short number, std::wstring_view param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, param.size(), 0, const_cast<wchar_t*>(param.data()), param.size(), nullptr)))
			assert(false);
		return *this;
	}

	DATABASE_API StoredProcedure& StoredProcedure::Out(unsigned short number, int32_t* param)
	{
		if (!SQL_SUCCEEDED(::SQLBindParameter(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_PARAM_OUTPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, param, 0, nullptr)))
			assert(false);
		return *this;
	}

	DATABASE_API StoredProcedure::Result StoredProcedure::Execute()
	{
		if (!m_stmt)
		{
			assert(false);
			return Result{ nullptr };
		}

		if (!SQL_SUCCEEDED(::SQLExecute(*m_stmt)))
		{
			assert(false);
			return Result{ nullptr };
		}

		// SP 반환값
		auto rc{ ::SQLMoreResults(*m_stmt) };
		if (!SQL_SUCCEEDED(rc))
		{
			assert(rc == SQL_NO_DATA);
			return Result{ nullptr };
		}

		return Result{ std::move(m_stmt) };
	}

	StoredProcedure::Result::Result(unique_stmt stmt)
	{
		m_stmt.swap(stmt);
	}

	DATABASE_API StoredProcedure::Result& StoredProcedure::Result::Bind(unsigned short number, int64_t* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_SBIGINT, param, 0, nullptr);
		return *this;
	}

	DATABASE_API StoredProcedure::Result& StoredProcedure::Result::Bind(unsigned short number, std::wstring* param)
	{
		::SQLBindCol(*m_stmt, static_cast<SQLUSMALLINT>(number), SQL_C_WCHAR, param->data(), param->size(), nullptr);
		return *this;
	}

	DATABASE_API StoredProcedure::Result& StoredProcedure::Result::Bind(unsigned short number, Time* param)
	{
		m_datetimes.emplace(param, datetime2{});
		::SQLBindCol(*m_stmt, number, SQL_C_TIMESTAMP, &m_datetimes[param], 0, nullptr);
		return *this;
	}

	DATABASE_API bool StoredProcedure::Result::Fetch()
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
