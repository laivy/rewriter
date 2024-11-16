#pragma once
#include "Types.h"

namespace Database
{
	class Session;

	using unique_stmt = std::unique_ptr<void*, std::function<void(void**)>>;
	using datetime2 = std::array<char, 16>; // sizeof(SQL_TIMESTAMP_STRUCT)

	class Query
	{
	public:
		class Result
		{
		public:
			Result(unique_stmt stmt);
			~Result() = default;

			DLL_API int64_t Return();
			DLL_API Result& Bind(unsigned short number, int32_t* param);
			DLL_API Result& Bind(unsigned short number, int64_t* param);
			DLL_API Result& Bind(unsigned short number, std::wstring* param);
			DLL_API Result& Bind(unsigned short number, Time* param);
			DLL_API bool Fetch();

		private:
			unique_stmt m_stmt;
			std::map<Time*, datetime2> m_datetimes;
		};

	public:
		DLL_API Query(Type type);
		DLL_API Query(Query& select);
		~Query() = default;

		DLL_API Query& Statement(std::wstring_view statement);
		DLL_API Query& Param(unsigned short number, int32_t param);
		DLL_API Query& Param(unsigned short number, int64_t param);
		DLL_API Query& Param(unsigned short number, std::wstring_view param);
		DLL_API Query& Param(unsigned short number, const Time& param);
		DLL_API Result Execute();

	private:
		Session* m_session;
		unique_stmt m_stmt;
	};
}
