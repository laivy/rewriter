#pragma once
#include "Common/Singleton.h"

namespace Database
{
	class Connection : public TSingleton<Connection>
	{
	public:
		Connection();
		~Connection() = default;

		void OnCreate();
		void OnDestroy();

	public:
		SQLHENV m_hEnv;
		SQLHDBC m_hDbc;
	};

	__declspec(dllexport) DBRESULT Connect(std::wstring_view server, std::wstring_view database, std::wstring_view username, std::wstring_view password);
}