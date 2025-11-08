#pragma once
#include "Export.h"
#include "Type.h"

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

			DATABASE_API std::int64_t Return();
			DATABASE_API Result& Bind(unsigned short number, int32_t* param);
			DATABASE_API Result& Bind(unsigned short number, int64_t* param);
			DATABASE_API Result& Bind(unsigned short number, std::wstring* param);
			DATABASE_API Result& Bind(unsigned short number, Time* param);
			DATABASE_API bool Fetch();

		private:
			unique_stmt m_stmt;
			std::map<Time*, datetime2> m_datetimes;
		};

	public:
		DATABASE_API Query(Type type);
		DATABASE_API Query(Query& query);
		~Query() = default;

		DATABASE_API Query& Statement(std::wstring_view statement);
		DATABASE_API Query& Param(unsigned short number, int32_t param);
		DATABASE_API Query& Param(unsigned short number, int64_t param);
		DATABASE_API Query& Param(unsigned short number, std::wstring_view param);
		DATABASE_API Query& Param(unsigned short number, const Time& param);
		DATABASE_API Result Execute();

	private:
		Session* m_session;
		unique_stmt m_stmt;
	};

	class StoredProcedure
	{
	public:
		class Result
		{
		public:
			Result(unique_stmt stmt);
			~Result() = default;

			DATABASE_API Result& Bind(unsigned short number, int64_t* param);
			DATABASE_API Result& Bind(unsigned short number, std::wstring* param);
			DATABASE_API Result& Bind(unsigned short number, Time* param);
			DATABASE_API bool Fetch();

		private:
			unique_stmt m_stmt;
			std::map<Time*, datetime2> m_datetimes;
		};

	public:
		DATABASE_API StoredProcedure(Type type);
		DATABASE_API StoredProcedure(StoredProcedure& sp);
		~StoredProcedure() = default;

		DATABASE_API StoredProcedure& Statement(std::wstring_view statement);
		DATABASE_API StoredProcedure& In(unsigned short number, int64_t param);
		DATABASE_API StoredProcedure& In(unsigned short number, std::wstring_view param);
		DATABASE_API StoredProcedure& Out(unsigned short number, int32_t* param);
		DATABASE_API Result Execute();

	private:
		Session* m_session;
		unique_stmt m_stmt;
	};
}
