#pragma once
#include "Types.h"

namespace Database
{
	class Session;

	class Query
	{
	public:
		DLL_API Query(Type type);
		DLL_API ~Query();

		DLL_API Query& Statement(std::wstring statement);
		DLL_API Query& Param(size_t number, int32_t param);
		DLL_API Query& Param(size_t number, int64_t param);
		DLL_API Query& Param(size_t number, const std::wstring& param);
		DLL_API bool Execute() const;

		DLL_API Query& Bind(size_t number, int32_t* param);
		DLL_API Query& Bind(size_t number, int64_t* param);
		DLL_API Query& Bind(size_t number, std::wstring* param);
		DLL_API bool Fetch() const;

	private:
		Session* m_session;
		SQLHSTMT m_stmt;
	};
}
