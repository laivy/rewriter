#pragma once
#include "Types.h"

namespace Database
{
	class Session;

	using unique_stmt = std::unique_ptr<void*, std::function<void(void**)>>;
	using datetime = std::array<char, 16>; // sizeof(SQL_TIMESTAMP_STRUCT)

	class Select
	{
	public:
		class Result
		{
		public:
			Result(unique_stmt stmt);
			~Result() = default;

			DLL_API Result& Bind(unsigned short number, int32_t* param);
			DLL_API Result& Bind(unsigned short number, int64_t* param);
			DLL_API Result& Bind(unsigned short number, std::wstring* param);
			DLL_API Result& Bind(unsigned short number, Time* param);
			DLL_API bool Fetch();

		private:
			unique_stmt m_stmt;
			std::map<Time*, datetime> m_datetimes;
		};

	public:
		DLL_API Select(Type type);
		DLL_API Select(Select& select);
		~Select() = default;

		DLL_API Select& Statement(std::wstring_view statement);
		DLL_API Select& Param(unsigned short number, int32_t param);
		DLL_API Select& Param(unsigned short number, int64_t param);
		DLL_API Select& Param(unsigned short number, std::wstring_view param);
		DLL_API Result Execute();

	private:
		Session* m_session;
		unique_stmt m_stmt;
	};
}
