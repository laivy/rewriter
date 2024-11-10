#pragma once

namespace Database
{
	using SPRESULT = SQLINTEGER;

	enum class DBRESULT
	{
		SUCCESS,
		ERR_NOT_INITIALIZED,
		ERR_CAN_NOT_CONNECT,
		ERR_CAN_NOT_ALLOC_HANDLE,
		ERR_UNKNOWN
	};

	enum class Database
	{
		None,
		Game,
		World
	};
}
